#include <bamf/transforms/ConstantFolder.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/support/Stack.hh>

namespace bamf {

namespace {

Constant *fold_binary(BinaryOp op, Constant *lhs, Constant *rhs) {
    switch (op) {
    case BinaryOp::Add:
        return new Constant(lhs->value() + rhs->value());
    case BinaryOp::Sub:
        return new Constant(lhs->value() - rhs->value());
    case BinaryOp::Shl:
        return new Constant(lhs->value() << rhs->value());
    }
    return nullptr;
}

} // namespace

void ConstantFolder::run_on(Function *function) {
    Stack<Instruction> work_queue;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            work_queue.push(inst.get());
        }
    }

    int folded_binary_count = 0;
    int folded_phi_count = 0;
    Stack<Instruction> remove_list;
    while (!work_queue.empty()) {
        auto *inst = work_queue.pop();

        // Fold binary instructions that have a constant value for both the LHS and RHS
        if (auto *binary = inst->as<BinaryInst>()) {
            auto *lhs = binary->lhs()->as<Constant>();
            if (lhs == nullptr) {
                continue;
            }

            auto *rhs = binary->rhs()->as<Constant>();
            if (rhs == nullptr) {
                continue;
            }

            for (auto *user : binary->users()) {
                if (auto *inst = user->as<Instruction>()) {
                    work_queue.push(inst);
                }
            }

            auto *folded = fold_binary(binary->op(), lhs, rhs);
            if (folded == nullptr) {
                m_logger.warn("Failed to fold constant binary instruction");
                continue;
            }
            binary->replace_all_uses_with(folded);
            remove_list.push(binary);
            folded_binary_count++;
        }

        // Propagate PHIs with only one incoming value
        if (auto *phi = inst->as<PhiInst>()) {
            if (phi->incoming().size() != 1) {
                continue;
            }

            for (auto *user : phi->users()) {
                if (auto *inst = user->as<Instruction>()) {
                    work_queue.push(inst);
                }
            }

            folded_phi_count++;
            for (auto [block, value] : *phi) {
                phi->replace_all_uses_with(value);
            }
            remove_list.push(phi);
        }
    }

    for (auto *inst : remove_list) {
        inst->remove_from_parent();
    }

    m_logger.trace("Folded {} binary instructions", folded_binary_count);
    m_logger.trace("Folded {} phi instructions", folded_phi_count);
}

} // namespace bamf
