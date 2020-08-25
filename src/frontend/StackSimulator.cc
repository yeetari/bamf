#include <bamf/frontend/StackSimulator.hh>

#include <bamf/core/DecompilationContext.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace bamf {

namespace {

struct StackOffset {
    struct Hasher {
        std::size_t operator()(const StackOffset &offset) const noexcept {
            auto h1 = std::hash<std::size_t>{}(offset.offset);
            auto h2 = std::hash<bool>{}(offset.negative);
            return h1 ^ h2;
        }
    };

    struct EqualFn {
        bool operator()(const StackOffset &lhs, const StackOffset &rhs) const noexcept {
            if (lhs.offset != rhs.offset) {
                return false;
            }
            return lhs.negative == rhs.negative;
        }
    };

    std::size_t offset{0};
    bool negative{false};

    StackOffset() = default;
    StackOffset(std::size_t offset, bool negative) : offset(offset), negative(negative) {}
};

// TODO: Build a proper tree (Graph<Value *>) of this
Value *find_root_value(Value *start) {
    if (start->is<AllocInst>()) {
        return start;
    }
    if (auto *binary = start->as<BinaryInst>()) {
        return find_root_value(binary->lhs());
    }
    if (auto *load = start->as<LoadInst>()) {
        return find_root_value(load->ptr());
    }
    if (auto *store = start->as<StoreInst>()) {
        return find_root_value(store->ptr());
    }

    assert(!start->is<Instruction>());
    return start;
}

} // namespace

void StackSimulator::run_on(Function *function) {
    std::unordered_map<LoadInst *, StackOffset> load_map;
    std::unordered_map<StoreInst *, StackOffset> store_map;
    std::unordered_map<StoreInst *, Value *> replace_map;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (!inst->is<LoadInst>() && !inst->is<StoreInst>()) {
                continue;
            }

            auto *root = find_root_value(inst.get());
            assert(root != nullptr);

            if (!m_decomp_ctx.is_phys_reg(root)) {
                continue;
            }

            auto *load = inst->as<LoadInst>();
            if (load != nullptr && !load->ptr()->is<BinaryInst>()) {
                continue;
            }

            auto *store = inst->as<StoreInst>();
            if (store != nullptr && !store->ptr()->is<BinaryInst>()) {
                continue;
            }

            auto *offset_inst = (load != nullptr ? load->ptr() : store->ptr())->as<BinaryInst>();
            assert(offset_inst != nullptr);

            auto *offset_const = offset_inst->rhs()->as<Constant>();
            assert(offset_const != nullptr);

            StackOffset offset = {offset_const->value(), offset_inst->op() == BinaryOp::Sub};
            if (load != nullptr) {
                load_map[load] = offset;
            }
            if (store != nullptr) {
                store_map[store] = offset;
                replace_map[store] = offset_inst;
            }
        }
    }

    std::unordered_map<StackOffset, AllocInst *, StackOffset::Hasher, StackOffset::EqualFn> alloc_map;
    for (int i = 0; auto [store, offset] : store_map) {
        if (!alloc_map.contains(offset)) {
            auto *alloc = function->entry()->prepend<AllocInst>();
            alloc->set_name("svar" + std::to_string(i++));
            alloc_map[offset] = alloc;
        }
        auto *alloc = alloc_map.at(offset);
        replace_map[store]->replace_all_uses_with(alloc);

        // TODO: Replace this with a Instruction::remove_from_parent()
        auto *val = store->val();
        auto position = store->parent()->remove(store);
        store->parent()->insert<StoreInst>(position, alloc, val);
    }

    for (auto [load, offset] : load_map) {
        auto *alloc = alloc_map[offset];
        auto *block = load->parent();
        auto *new_load = block->insert<LoadInst>(block->position_of(load), alloc);
        load->replace_all_uses_with(new_load);
        load->remove_from_parent();
    }

    m_logger.trace("Created {} allocs", alloc_map.size());
    m_logger.trace("Replaced {} loads", load_map.size());
    m_logger.trace("Replaced {} stores", store_map.size());
}

} // namespace bamf
