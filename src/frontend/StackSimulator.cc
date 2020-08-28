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

// TODO: Build a proper tree (Graph<Value *>) of this.
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
    std::unordered_map<LoadInst *, std::int32_t> load_map;
    std::unordered_map<StoreInst *, std::int32_t> store_map;
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

            auto offset = static_cast<std::int32_t>(offset_const->value());
            if (load != nullptr) {
                assert(!load_map.contains(load));
                load_map.emplace(load, offset);
            }
            if (store != nullptr) {
                assert(!store_map.contains(store));
                assert(!replace_map.contains(store));
                store_map.emplace(store, offset);
                replace_map.emplace(store, offset_inst);
            }
        }
    }

    std::unordered_map<std::int32_t, AllocInst *> alloc_map;
    for (int i = 0; auto [store, offset] : store_map) {
        auto *&alloc = alloc_map[offset];
        if (alloc == nullptr) {
            alloc = function->entry()->prepend<AllocInst>();
            alloc->set_name("svar" + std::to_string(i++));
        }

        auto *val = store->val();
        auto position = store->parent()->remove(store);
        store->parent()->insert<StoreInst>(position, alloc, val);
        replace_map[store]->replace_all_uses_with(alloc);
    }

    for (auto [load, offset] : load_map) {
        auto *alloc = alloc_map.at(offset);
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
