#include <bamf/frontend/RegisterLocaliser.hh>

#include <bamf/core/DecompilationContext.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/GlobalVariable.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/ir/Program.hh>

#include <unordered_map>

namespace bamf {

void RegisterLocaliser::run_on(Program *program) {
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
                alloc_map.emplace(function, alloc);
                for (auto *new_user : reg->users()) {
                    alloc->add_user(new_user);
                }
            }
            inst->replace_uses_of_with(reg, alloc_map.at(function));
        }
        // TODO: Hacky, replace_uses_of_with() should remove itself as user and add new users.
        reg->replace_all_uses_with(nullptr);
        program->remove_global(reg);
    }
}

} // namespace bamf
