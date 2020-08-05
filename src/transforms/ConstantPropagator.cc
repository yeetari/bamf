#include <bamf/transforms/ConstantPropagator.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>

#include <unordered_map>
#include <vector>

namespace bamf {

namespace {

struct VarInfo {
    std::vector<LoadInst *> loads;
    std::vector<StoreInst *> stores;
};

void run(BasicBlock *block, std::unordered_map<Value *, VarInfo> *map) {
    // Build def-use info
    auto &info_map = *map;
    for (auto &inst : *block) {
        if (auto *load = inst->as<LoadInst>()) {
            info_map[load->ptr()].loads.push_back(load);
        } else if (auto *store = inst->as<StoreInst>()) {
            info_map[store->dst()].stores.push_back(store);
        }
    }

    for (auto &[var, info] : info_map) {
        // If a var only has one store (def), we can propagate the load values with the store value
        // NOTE: The dead instructions will stay after this pass, you must run the DeadInstructionPruner pass
        if (info.stores.size() == 1) {
            auto *store = info.stores[0];
            for (auto *load : info.loads) {
                load->replace_all_uses_with(store->src());
            }
        }
    }
}

} // namespace

void ConstantPropagator::run_on(Function *function) {
    std::unordered_map<Value *, VarInfo> info_map;
    for (auto &block : *function) {
        run(block.get(), &info_map);
    }
}

} // namespace bamf
