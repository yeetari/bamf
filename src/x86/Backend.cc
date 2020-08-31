#include <bamf/x86/Backend.hh>

#include <bamf/backend/BackendInstVisitor.hh>
#include <bamf/backend/BackendInstructions.hh>
#include <bamf/backend/MoveInserter.hh>
#include <bamf/backend/RegAllocator.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/ir/Program.hh>
#include <bamf/pass/PassUsage.hh>
#include <bamf/x86/MachineInst.hh>

#include <cassert>
#include <cstdint>
#include <vector>

namespace bamf::x86 {

namespace {

class InstTranslator : public BackendInstVisitor {
    friend Backend;

private:
    class Builder {
        MachineInst *const m_inst;
        std::uint8_t m_op_count{};

    public:
        explicit Builder(MachineInst *inst) : m_inst(inst) {}

        Builder &base_disp(Register base, std::uint32_t disp);
        Builder &imm(std::uint64_t op);
        Builder &reg(Register op);
    };

    std::vector<MachineInst> m_insts;

    Builder emit(Opcode opcode);

public:
    void visit(AllocInst *) override;
    void visit(BinaryInst *) override;
    void visit(BranchInst *) override;
    void visit(CompareInst *) override;
    void visit(CondBranchInst *) override;
    void visit(LoadInst *) override;
    void visit(MoveInst *) override;
    void visit(PhiInst *) override;
    void visit(StoreInst *) override;
    void visit(RetInst *) override;
};

InstTranslator::Builder &InstTranslator::Builder::base_disp(Register base, std::uint32_t disp) {
    m_inst->operands[m_op_count].type = OperandType::MemBaseDisp;
    m_inst->operands[m_op_count].base = base;
    m_inst->operands[m_op_count++].disp = disp;
    return *this;
}

InstTranslator::Builder &InstTranslator::Builder::imm(std::uint64_t op) {
    m_inst->operands[m_op_count].type = OperandType::Imm;
    m_inst->operands[m_op_count++].imm = op;
    return *this;
}

InstTranslator::Builder &InstTranslator::Builder::reg(Register op) {
    m_inst->operands[m_op_count].type = OperandType::Reg;
    m_inst->operands[m_op_count++].reg = op;
    return *this;
}

InstTranslator::Builder InstTranslator::emit(Opcode opcode) {
    auto &inst = m_insts.emplace_back();
    inst.opcode = opcode;
    inst.address_width = 64;
    inst.operand_width = 64;
    return InstTranslator::Builder(&inst);
}

void InstTranslator::visit(AllocInst *) {
    assert(false);
}

void InstTranslator::visit(BinaryInst *) {
    assert(false);
}

void InstTranslator::visit(BranchInst *) {
    assert(false);
}

void InstTranslator::visit(CompareInst *) {
    assert(false);
}

void InstTranslator::visit(CondBranchInst *) {
    assert(false);
}

void InstTranslator::visit(LoadInst *) {
    assert(false);
}

void InstTranslator::visit(MoveInst *move) {
    auto *constant = move->val()->as<Constant>();
    assert(constant != nullptr);
    emit(Opcode::Mov).reg(static_cast<Register>(move->dst())).imm(constant->value());
}

void InstTranslator::visit(PhiInst *) {
    assert(false);
}

void InstTranslator::visit(StoreInst *) {
    assert(false);
}

void InstTranslator::visit(RetInst *) {
    emit(Opcode::Ret);
}

} // namespace

void Backend::build_usage(PassUsage *usage) {
    usage->depends_on<MoveInserter>();
    usage->depends_on<RegAllocator>();
}

void Backend::run_on(Program *program) {
    InstTranslator translator;
    for (auto &function : *program) {
        for (auto &block : *function) {
            for (auto &inst : *block) {
                inst->accept(&translator);
            }
        }
    }
    for (auto &inst : translator.m_insts) {
        dump_inst(inst, false);
    }
}

} // namespace bamf::x86
