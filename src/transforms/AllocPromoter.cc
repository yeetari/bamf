#include <bamf/transforms/AllocPromoter.hh>

#include <bamf/graph/DepthFirstSearch.hh>
#include <bamf/graph/DominanceComputer.hh>
#include <bamf/graph/DominatorTree.hh>
#include <bamf/graph/Graph.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
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

bool run(Function *function, const Statistic &propagated_load_count, const Statistic &pruned_store_count) {
    // Build CFG
    Graph<BasicBlock> cfg;
    cfg.set_entry(function->entry());
    for (auto &block : *function) {
        for (auto &inst : *block) {
            // TODO: Make Edge<V> the default template arg for connect
            if (auto *branch = inst->as<BranchInst>()) {
                cfg.connect<Edge<BasicBlock>>(branch->parent(), branch->dst());
            } else if (auto *cond_branch = inst->as<CondBranchInst>()) {
                cfg.connect<Edge<BasicBlock>>(cond_branch->parent(), cond_branch->false_dst());
                cfg.connect<Edge<BasicBlock>>(cond_branch->parent(), cond_branch->true_dst());
            }
        }
    }

    // Build dominance frontiers
    // TODO: Move this into DominatorTree
    auto tree = cfg.run<DominanceComputer>();
    auto tree_dfs = tree.run<DepthFirstSearch>();
    std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> frontiers;
    for (auto *post_idom : tree_dfs.post_order()) {
        //
        for (auto *succ : cfg.succs_of(post_idom)) {
            if (tree.idom(succ) != post_idom) {
                frontiers[post_idom].insert(succ);
            }
        }

        //
        for (auto *f : tree.succs_of(post_idom)) {
            for (auto *ff : frontiers[f]) {
                if (tree.idom(ff) != post_idom) {
                    frontiers[post_idom].insert(ff);
                }
            }
        }
    }

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

    for (auto &[var, info] : vars) {
        std::unordered_set<BasicBlock *> visited;
        for (auto *store : info.stores) {
            for (auto *df : frontiers[store->parent()]) {
                if (visited.contains(df)) {
                    continue;
                }
                visited.insert(df);
                // TODO: A phi insertion count statistic
                auto *phi = df->prepend<PhiInst>();
                reverse_map[phi] = var;
            }
        }
    }

    bool changed = false;
    Stack<BasicBlock> work_queue;
    work_queue.push(cfg.entry());
    while (!work_queue.empty()) {
        auto *block = work_queue.pop();
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
                    ++propagated_load_count;
                }
            } else if (auto *phi = inst->as<PhiInst>()) {
                def_stack.push(phi);
            } else if (auto *store = inst->as<StoreInst>()) {
                def_stack.push(store->val());
            }
        }

        for (auto *succ : cfg.succs_of(block)) {
            for (auto &inst : *succ) {
                if (auto *phi = inst->as<PhiInst>()) {
                    if (!reverse_map.contains(phi)) {
                        continue;
                    }
                    auto &var_info = vars.at(reverse_map.at(phi));
                    auto &def_stack = var_info.def_stack;
                    if (!def_stack.empty()) {
                        phi->add_incoming(block, def_stack.peek());
                    }
                }
            }
        }

        for (auto *succ : tree.succs_of(block)) {
            work_queue.push(succ);
        }
    }

    for (auto &[var, info] : vars) {
        // Remove (now dead) stores. We have to do this here, since the dead instruction pruner pass won't be able to
        // tell that this store is dead (since it technically still has uses).
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

void AllocPromoter::run_on(Function *function) {
    bool changed = false;
    Statistic propagated_load_count(m_logger, "Propagated {} loads");
    Statistic pruned_store_count(m_logger, "Pruned {} stores");
    do {
        changed = run(function, propagated_load_count, pruned_store_count);
    } while (changed);
}

} // namespace bamf
