#include <bamf/transforms/RegisterLocaliser.hh>

#include <bamf/core/DecompilationContext.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/GlobalVariable.hh>
#include <bamf/ir/Instructions.hh>

#include <unordered_map>

namespace bamf {

void RegisterLocaliser::run_on(Program *) {
    for (auto *reg : m_decomp_ctx.phys_regs()) {
        std::unordered_map<Function *, AllocInst *> alloc_map;
        for (auto *user : reg->users()) {
            auto *inst = user->as<Instruction>();
            if (inst == nullptr) {
                continue;
            }

            auto *function = inst->parent()->parent();
            auto *entry = function->entry();
            if (!alloc_map.contains(function)) {
                auto *alloc = entry->prepend<AllocInst>();
                alloc->set_name(reg->name());
                alloc_map[function] = alloc;
            }

            // TODO: This isn't right, this should be `inst->replace_uses_of_with(reg, alloc_map[function])`
            reg->replace_all_uses_with(alloc_map[function]);
        }
    }
}

} // namespace bamf
