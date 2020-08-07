#include <bamf/transforms/StackSimulator.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/GlobalVariable.hh>
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
    if (auto *binary = start->as<BinaryInst>()) {
        return find_root_value(binary->lhs());
    }
    if (auto *constant = start->as<Constant<std::size_t>>()) {
        return constant;
    }
    if (auto *global = start->as<GlobalVariable>()) {
        return global;
    }
    if (auto *load = start->as<LoadInst>()) {
        return find_root_value(load->ptr());
    }
    if (auto *store = start->as<StoreInst>()) {
        return find_root_value(store->dst());
    }
    return nullptr;
}

} // namespace

void StackSimulator::run_on(Function *function) {
    std::unordered_map<LoadInst *, StackOffset> load_map;
    std::unordered_map<StoreInst *, StackOffset> store_map;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (!inst->is<LoadInst>() && !inst->is<StoreInst>()) {
                continue;
            }

            auto *root = find_root_value(inst.get());
            assert(root != nullptr);

            // TODO: Hacky, this should work for any GPR
            if (!root->is<GlobalVariable>() || root->name() != "rbp") {
                continue;
            }

            auto *load = inst->as<LoadInst>();
            auto *store = inst->as<StoreInst>();

            if (load != nullptr && !load->ptr()->is<BinaryInst>()) {
                continue;
            }

            auto *offset_inst = (load != nullptr ? load->ptr() : store->dst())->as<BinaryInst>();
            assert(offset_inst != nullptr);

            auto *offset_const = offset_inst->rhs()->as<Constant<std::size_t>>();
            assert(offset_const != nullptr);

            StackOffset offset = {offset_const->value(), offset_inst->op() == BinaryOp::Sub};
            if (load != nullptr) {
                load_map[load] = offset;
            }
            if (store != nullptr) {
                store_map[store] = offset;
            }
        }
    }

    std::unordered_map<StackOffset, AllocInst *, StackOffset::Hasher, StackOffset::EqualFn> alloc_map;
    for (auto [store, offset] : store_map) {
        auto *alloc = function->entry()->prepend<AllocInst>();
        alloc->set_name("svar" + std::to_string(offset.offset));
        alloc_map[offset] = alloc;

        // TODO: Replace this with a Instruction::remove_from_parent()
        auto *src = store->src();
        auto position = store->parent()->remove(store);
        store->parent()->insert<StoreInst>(position, alloc, src);
    }

    for (auto [load, offset] : load_map) {
        // TODO: This is a mess
        auto *alloc = alloc_map[offset];
        auto old_uses = load->uses();
        auto position = load->parent()->remove(load);
        auto *new_load = load->parent()->insert<LoadInst>(position, alloc);

        for (auto **use : old_uses) {
            *use = new_load;
            new_load->add_use(use);
        }
    }

    m_logger.trace("Created {} allocs", alloc_map.size());
    m_logger.trace("Replaced {} loads", load_map.size());
    m_logger.trace("Replaced {} stores", store_map.size());
}

} // namespace bamf
