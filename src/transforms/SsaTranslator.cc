#include <bamf/transforms/SsaTranslator.hh>

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
    std::vector<LoadInst *> loads;
    std::vector<StoreInst *> stores;
};

} // namespace

void SsaTranslator::run_on(Function *function) {
    // Build def-use info
    // TODO: Create a def-use/load-store analysis pass
    std::unordered_map<AllocInst *, VarInfo> info_map;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (auto *load = inst->as<LoadInst>()) {
                auto *alloc = load->ptr()->as<AllocInst>();
                if (alloc != nullptr) {
                    info_map[alloc].loads.push_back(load);
                }
            } else if (auto *store = inst->as<StoreInst>()) {
                auto *alloc = store->dst()->as<AllocInst>();
                if (alloc != nullptr) {
                    info_map[alloc].stores.push_back(store);
                }
            }
        }
    }

    // TODO: Use def-use info here!
    std::unordered_map<AllocInst *, StoreInst *> defs;
    std::unordered_map<LoadInst *, StoreInst *> reaching_defs;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (auto *load = inst->as<LoadInst>()) {
                auto *alloc = load->ptr()->as<AllocInst>();
                if (alloc == nullptr) {
                    continue;
                }
                if (!defs.contains(alloc)) {
                    continue;
                }
                reaching_defs[load] = defs[alloc];
            }
            if (auto *store = inst->as<StoreInst>()) {
                auto *alloc = store->dst()->as<AllocInst>();
                if (alloc == nullptr) {
                    continue;
                }
                defs[alloc] = store;
            }
        }
    }

    int vars_created = 0;
    int replaced_loads = 0;
    int pruned_stores = 0;
    for (auto &[var, info] : info_map) {
        // Variable already only has one store, no need to split it
        // These stores will most likely be eliminated by the DeadStorePruner transform
        if (info.stores.size() <= 1) {
            continue;
        }

        auto *entry = var->parent()->parent()->entry();
        std::unordered_map<StoreInst *, AllocInst *> new_vars;
        for (auto *store : info.stores) {
            auto *new_var = entry->prepend<AllocInst>();
            new_vars[store] = new_var;
            if (var->has_name()) {
                new_var->set_name(var->name() + "_" + std::to_string(vars_created));
            }
            vars_created++;
        }

        for (auto *load : info.loads) {
            assert(reaching_defs.contains(load));
            auto *block = load->parent();
            auto *reaching_def = reaching_defs[load];
            assert(new_vars.contains(reaching_def));
            block->insert<StoreInst>(block->position_of(load), new_vars[reaching_def], reaching_def->src());

            auto *new_load = block->insert<LoadInst>(block->position_of(load), new_vars[reaching_def]);
            load->replace_all_uses_with(new_load);
            replaced_loads++;
        }

        for (auto *store : info.stores) {
            store->remove_from_parent();
            pruned_stores++;
        }
    }

    m_logger.trace("Created {} new variables", vars_created);
    m_logger.trace("Replaced {} loads", replaced_loads);
    m_logger.trace("Pruned {} stores", pruned_stores);
}

} // namespace bamf
