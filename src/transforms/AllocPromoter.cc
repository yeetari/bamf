#include <bamf/transforms/AllocPromoter.hh>

#include <bamf/analyses/ControlFlowAnalyser.hh>
#include <bamf/analyses/ControlFlowAnalysis.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/PassUsage.hh>
#include <bamf/pass/Statistic.hh>
#include <bamf/support/Stack.hh>

#include <cassert>
#include <unordered_map>
#include <unordered_set>
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

bool run(Function *function, ControlFlowAnalysis *cfa, const Statistic &propagated_load_count,
         const Statistic &pruned_store_count) {
    // Build store-load info for each variable.
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

    // Insert phis.
    for (auto &[var, info] : vars) {
        std::unordered_set<BasicBlock *> visited;
        for (auto *store : info.stores) {
            for (auto *df : cfa->frontiers(store->parent())) {
                if (!visited.insert(df).second) {
                    continue;
                }
                // TODO: A phi insertion count statistic.
                auto *phi = df->prepend<PhiInst>();
                reverse_map[phi] = var;
            }
        }
    }

    // Iteratively replace all uses of every load of a var with the reaching def of the var (either a phi or RHS of a
    // store). This has the side effect of making all loads and stores dead.
    bool changed = false;
    Stack<BasicBlock> work_queue;
    work_queue.push(cfa->entry());
    while (!work_queue.empty()) {
        auto *block = work_queue.pop();
        for (auto &i : *block) {
            auto *inst = i.get();
            if (!reverse_map.contains(inst)) {
                continue;
            }

            // TODO: Avoid allocating a Stack<Value> for single store allocs.
            auto &info = vars.at(reverse_map.at(inst));
            auto &def_stack = info.def_stack;
            if (auto *load = inst->as<LoadInst>()) {
                // Don't propagate if there is no def.
                // TODO: Assert here?
                if (def_stack.empty()) {
                    continue;
                }
                // Load will become trivially dead.
                load->replace_all_uses_with(def_stack.peek());
                changed = true;
                ++propagated_load_count;
            } else if (auto *phi = inst->as<PhiInst>()) {
                def_stack.push(phi);
            } else if (auto *store = inst->as<StoreInst>()) {
                def_stack.push(store->val());
            }
        }

        // Add this block as an incoming value to all phis of successor blocks.
        for (auto *succ : cfa->succs(block)) {
            for (auto &inst : *succ) {
                auto *phi = inst->as<PhiInst>();
                if (phi == nullptr || !reverse_map.contains(phi)) {
                    continue;
                }
                auto &var_info = vars.at(reverse_map.at(phi));
                auto &def_stack = var_info.def_stack;
                if (!def_stack.empty()) {
                    phi->add_incoming(block, def_stack.peek());
                }
            }
        }

        // Add blocks that this block immediately dominates to queue.
        for (auto *idom : cfa->tree_succs(block)) {
            work_queue.push(idom);
        }
    }

    // Remove the now dead stores. We have to do this here, since the dead instruction pruner pass won't be able to
    // tell that this store is dead (since it technically still has uses).
    for (auto &[var, info] : vars) {
        for (auto *store : info.stores) {
            assert(store->users().empty());
            store->remove_from_parent();
            changed = true;
            ++pruned_store_count;
        }
    }
    return changed;
}

} // namespace

void AllocPromoter::build_usage(PassUsage *usage) {
    usage->depends_on<ControlFlowAnalyser>();
}

void AllocPromoter::run_on(Function *function) {
    Statistic propagated_load_count(m_logger, "Propagated {} loads");
    Statistic pruned_store_count(m_logger, "Pruned {} stores");
    for (bool changed = true; changed;) {
        auto *cfa = m_manager->get<ControlFlowAnalysis>(function);
        changed = run(function, cfa, propagated_load_count, pruned_store_count);
    }
}

} // namespace bamf
