#include <bamf/transforms/ConstantBranchEvaluator.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/Statistic.hh>

namespace bamf {

void ConstantBranchEvaluator::run_on(Function *function) {
    Statistic evaluated_count(m_logger, "Evaluated {} conditional branches");
    for (auto &block : *function) {
        for (auto &inst : *block) {
            auto *cond_branch = inst->as<CondBranchInst>();
            if (cond_branch == nullptr) {
                continue;
            }

            auto *constant = cond_branch->cond()->as<Constant>();
            if (constant == nullptr) {
                continue;
            }

            auto *new_dst = constant->value() == 1 ? cond_branch->true_dst() : cond_branch->false_dst();
            block->append<BranchInst>(new_dst);
            block->remove(cond_branch);
            ++evaluated_count;
        }
    }
}

} // namespace bamf
