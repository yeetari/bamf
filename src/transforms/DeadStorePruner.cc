#include <bamf/transforms/DeadStorePruner.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <map>
#include <vector>

namespace bamf {

namespace {

struct VarInfo {
    std::vector<LoadInst *> loads;
    std::vector<StoreInst *> stores;
};

bool run(BasicBlock *block, std::map<Value *, VarInfo> *map, int *pruned_count) {
    // Build def-use info
    auto &info_map = *map;
    for (auto &inst : *block) {
        if (auto *load = inst->as<LoadInst>()) {
            info_map[load->ptr()].loads.push_back(load);
        } else if (auto *store = inst->as<StoreInst>()) {
            info_map[store->dst()].stores.push_back(store);
        }
    }

    bool changed = false;
    for (auto &[var, info] : info_map) {
        // If a var only has one store (def), we can propagate the load values with the store value
        // NOTE: The dead loads will stay after this pass, you must run the TriviallyDeadInstPruner pass
        if (info.stores.size() == 1) {
            auto *store = info.stores[0];
            for (auto *load : info.loads) {
                // Propagate all uses of the load with the RHS of the store
                changed = true;
                load->replace_all_uses_with(store->src());
                var->remove_user(load);
            }

            // Something other than the loads/store is using this var, we can't remove it
            if (var->users().size() >= 2) {
                continue;
            }

            // Remove dead store
            assert(store->uses().empty());
            block->remove(store);
            changed = true;
            (*pruned_count)++;
        }
    }
    return changed;
}

} // namespace

void DeadStorePruner::run_on(Function *function) {
    int pruned_count = 0;
    bool changed = false;
    do {
        changed = false;
        std::map<Value *, VarInfo> info_map;
        for (auto &block : *function) {
            changed |= run(block.get(), &info_map, &pruned_count);
        }
    } while (changed);
    m_logger.trace("Pruned {} dead stores", pruned_count);
}

} // namespace bamf
