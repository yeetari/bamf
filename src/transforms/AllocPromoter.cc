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
    Stack<Value> def_stack;
    std::vector<StoreInst *> stores;
};

bool is_dead(AllocInst *alloc) {
    for (auto *user : alloc->users()) {
        if (user->is<StoreInst>()) {
            return false;
        }
    }
    return true;
}

bool is_promotable(AllocInst *alloc) {
    for (auto *user : alloc->users()) {
        if (auto *store = user->as<StoreInst>()) {
            if (store->ptr() != alloc) {
                return false;
            }
        }
    }
    return true;
}

bool run(Function *function, int *propagated_load_count, int *pruned_store_count) {
    // Build store-load (def-use) info
    std::unordered_map<AllocInst *, VarInfo> vars;
    std::unordered_map<Value *, AllocInst *> reverse_map;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            auto *alloc = inst->as<AllocInst>();
            if (alloc == nullptr) {
                continue;
            }

            // Ignore dead variable. If it trivially dead (i.e. no uses at all), it will be removed by the
            // TriviallyDeadInstPruner pass.
            if (is_dead(alloc)) {
                continue;
            }

            // Some variables can't be promoted, e.g. if its address is taken (it is on the RHS of a store).
            if (!is_promotable(alloc)) {
                continue;
            }

            assert(!vars.contains(alloc));
            for (auto *user : alloc->users()) {
                reverse_map[user] = alloc;
                if (auto *store = user->as<StoreInst>()) {
                    vars[alloc].stores.push_back(store);
                }
            }
        }
    }

    bool changed = false;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (!reverse_map.contains(inst.get())) {
                continue;
            }

            // TODO: Avoid allocating a Stack<Value> for single store allocs
            auto &info = vars.at(reverse_map.at(inst.get()));
            auto &def_stack = info.def_stack;
            if (auto *load = inst->as<LoadInst>()) {
                if (!def_stack.empty()) {
                    // Load will become trivially dead
                    load->replace_all_uses_with(def_stack.peek());

                    changed = true;
                    (*propagated_load_count)++;
                }
            } else if (auto *store = inst->as<StoreInst>()) {
                def_stack.push(store->val());
            }
        }
    }

    for (auto &[var, info] : vars) {
        // Remove (now dead) stores. We have to do this here, since the dead instruction pruner pass won't be able to
        // tell that this store is dead (since it technically still has uses).
        for (auto *store : info.stores) {
            assert(store->users().empty());
            store->remove_from_parent();

            changed = true;
            (*pruned_store_count)++;
        }
    }

    return changed;
}

} // namespace

void AllocPromoter::run_on(Function *function) {
    int propagated_load_count = 0;
    int pruned_store_count = 0;

    bool changed = false;
    do {
        changed = run(function, &propagated_load_count, &pruned_store_count);
    } while (changed);

    m_logger.trace("Propagated {} loads", propagated_load_count);
    m_logger.trace("Pruned {} stores", pruned_store_count);
}

} // namespace bamf
