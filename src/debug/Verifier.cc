#include <bamf/debug/Verifier.hh>

#include <bamf/analyses/ControlFlowAnalyser.hh>
#include <bamf/analyses/ControlFlowAnalysis.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/PassUsage.hh>

namespace bamf {

void Verifier::build_usage(PassUsage *usage) {
    usage->depends_on<ControlFlowAnalyser>();
}

void Verifier::run_on(Function *function) {
    auto *cfa = m_manager->get<ControlFlowAnalysis>(function);
    for (auto &block : *function) {
        for (auto &inst : *block) {
            // Ensure that an instruction dominates all of its uses.
            for (auto *user : inst->users()) {
                if (auto *user_inst = user->as<Instruction>()) {
                    // Ignore phis for now.
                    if (user_inst->is<PhiInst>()) {
                        continue;
                    }
                    if (!cfa->dominates(inst.get(), user_inst)) {
                        m_logger.error("Instruction does not dominate all uses!");
                    }
                }
            }
        }
    }
}

} // namespace bamf
