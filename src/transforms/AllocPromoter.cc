#include <bamf/transforms/AllocPromoter.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/support/Stack.hh>

#include <cassert>
#include <unordered_map>
#include <vector>

namespace bamf {

namespace {

struct VarInfo {
    std::vector<StoreInst *> defs;
    std::vector<LoadInst *> uses;
};

void promote_single_store(const VarInfo &info, int *propagated_load_count) {
    auto *store = info.defs[0];
    for (auto *load : info.uses) {
        auto *block = load->parent();
        if (block != store->parent()) {
            continue;
        }

        // Don't propagate loads that are before the store
        if (std::distance(block->position_of(load), block->position_of(store)) > 0) {
            continue;
        }

        assert(store->src() != load);
        load->replace_all_uses_with(store->src());
        (*propagated_load_count)++;
    }

    assert(store->users().empty());
    store->remove_from_parent();
}

} // namespace

// TODO: Rework some of the terminology here? A use in def-use info could be confused with a value's uses.
void AllocPromoter::run_on(Function *function) {
    // Build def-use (store-load) info.
    std::unordered_map<AllocInst *, VarInfo> vars;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (auto *load = inst->as<LoadInst>()) {
                if (auto *alloc = load->ptr()->as<AllocInst>()) {
                    vars[alloc].uses.push_back(load);
                }
            } else if (auto *store = inst->as<StoreInst>()) {
                if (auto *alloc = store->dst()->as<AllocInst>()) {
                    vars[alloc].defs.push_back(store);
                }
            }
        }
    }

    int propagated_load_count = 0;
    int pruned_store_count = 0;
    for (auto &[var, info] : vars) {
        // Ignore dead variable. If it trivially dead (i.e. no uses at all), it will be removed by the
        // TriviallyDeadInstPruner pass.
        if (info.defs.empty()) {
            continue;
        }

        // If a var only has one definition (store), we can propagate all uses of the loads with the RHS of the store.
        // Dead allocs or loads are not pruned by this pass. Normally you would want a dead instruction pruner pass to
        // run sometime after this.
        if (info.defs.size() == 1) {
            promote_single_store(info, &propagated_load_count);
            pruned_store_count++;
            continue;
        }

        // Else we have to fall back to standard SSA construction. Note that this pass does not modify any memory
        // accesses explicitly, it simply promotes trivially propagatable allocs. Bamf IR is similar to LLVM in that SSA
        // form is only used for scalar registers (and not memory).
        Stack<Value> def_stack;
        for (auto &inst : *var->parent()) {
            if (auto *load = inst->as<LoadInst>()) {
                if (!def_stack.empty()) {
                    // Load will become trivially dead.
                    load->replace_all_uses_with(def_stack.peek());
                    propagated_load_count++;
                }
            } else if (auto *store = inst->as<StoreInst>()) {
                def_stack.push(store->src());
            }
        }

        // Remove (now dead) stores. We have to do this here, since the dead instruction pruner pass won't be able to
        // tell that this store is dead (since it technically still has uses).
        for (auto *store : info.defs) {
            assert(store->users().empty());
            store->remove_from_parent();
            pruned_store_count++;
        }
    }

    m_logger.trace("Propagated {} loads", propagated_load_count);
    m_logger.trace("Pruned {} stores", pruned_store_count);
}

} // namespace bamf
