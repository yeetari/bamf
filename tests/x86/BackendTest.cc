#include <bamf/ir/Constant.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/ir/Program.hh>
#include <bamf/pass/PassManager.hh>
#include <bamf/x86/Backend.hh>
#include <bamf/x86/BackendResult.hh>
#include <bamf/x86/Builder.hh>

#include <gtest/gtest.h>

#include <tuple>

namespace bamf::x86 {

std::ostream &operator<<(std::ostream &stream, const MachineInst &inst) {
    return stream << dump_inst(inst, false);
}

namespace {

#define EXPECT(op) Builder(&expected.emplace_back()).opcode(op).width(64, 64)
#define RUN(expected)                                                                                                  \
    PassManager pass_manager;                                                                                          \
    pass_manager.add<Backend>();                                                                                       \
    pass_manager.run(program.get());                                                                                   \
    auto *actual = pass_manager.get<BackendResult>(function);                                                          \
    ASSERT_EQ(actual->size(), expected.size());                                                                        \
    for (int i = 0; i < actual->size(); i++) {                                                                         \
        EXPECT_EQ((*actual)[i], expected[i]);                                                                          \
    }

std::tuple<std::unique_ptr<Program>, Function *, BasicBlock *> create_prog() {
    auto program = std::make_unique<Program>();
    auto *function = program->create_function("main");
    auto *entry = function->append_block();
    program->set_main(function);
    return std::make_tuple(std::move(program), function, entry);
}

TEST(x86BackendTest, RetConstant) {
    auto [program, function, entry] = create_prog();
    entry->append<RetInst>(new Constant(10));

    std::vector<MachineInst> expected;
    EXPECT(Opcode::Label).label(0);
    EXPECT(Opcode::Push).reg(Register::Rbp);
    EXPECT(Opcode::Mov).reg(Register::Rbp).reg(Register::Rsp);
    EXPECT(Opcode::Mov).reg(Register::Rax).imm(10);
    EXPECT(Opcode::Pop).reg(Register::Rbp);
    EXPECT(Opcode::Ret);
    RUN(expected);
}

TEST(x86BackendTest, SimpleLoop) {
    auto [program, function, entry] = create_prog();
    auto *body = function->append_block();
    auto *latch = function->append_block();
    auto *exit = function->append_block();
    entry->append<BranchInst>(body);

    auto *count_phi = body->append<PhiInst>();
    count_phi->add_incoming(entry, new Constant(0));
    auto *cmp = body->append<CompareInst>(ComparePred::Eq, count_phi, new Constant(10));
    body->append<CondBranchInst>(cmp, latch, exit);

    auto *count_add = latch->append<BinaryInst>(BinaryOp::Add, count_phi, new Constant(1));
    count_phi->add_incoming(latch, count_add);
    latch->append<BranchInst>(body);
    exit->append<RetInst>(count_phi);

    std::vector<MachineInst> expected;
    EXPECT(Opcode::Label).label(0);
    EXPECT(Opcode::Push).reg(Register::Rbp);
    EXPECT(Opcode::Mov).reg(Register::Rbp).reg(Register::Rsp);
    EXPECT(Opcode::Mov).base_disp(Register::Rbp, -32).imm(0);
    EXPECT(Opcode::Jmp).label(1);
    EXPECT(Opcode::Label).label(1);
    EXPECT(Opcode::Mov).reg(Register::Rax).base_disp(Register::Rbp, -32);
    EXPECT(Opcode::Mov).reg(Register::Rcx).imm(10);
    EXPECT(Opcode::Cmp).reg(Register::Rax).reg(Register::Rcx);
    EXPECT(Opcode::Sete).width(64, 8).reg(Register::Rax);
    EXPECT(Opcode::Mov).base_disp(Register::Rbp, -24).reg(Register::Rax);
    EXPECT(Opcode::Mov).reg(Register::Rcx).base_disp(Register::Rbp, -24);
    EXPECT(Opcode::Mov).base_disp(Register::Rbp, -16).reg(Register::Rcx);
    EXPECT(Opcode::Cmp).base_disp(Register::Rbp, -16).imm(1);
    EXPECT(Opcode::Jne).label(2);
    EXPECT(Opcode::Jmp).label(3);
    EXPECT(Opcode::Label).label(2);
    EXPECT(Opcode::Mov).reg(Register::Rax).base_disp(Register::Rbp, -32);
    EXPECT(Opcode::Mov).reg(Register::Rcx).imm(1);
    EXPECT(Opcode::Add).reg(Register::Rax).reg(Register::Rcx);
    EXPECT(Opcode::Mov).base_disp(Register::Rbp, -8).reg(Register::Rax);
    EXPECT(Opcode::Mov).reg(Register::Rcx).base_disp(Register::Rbp, -8);
    EXPECT(Opcode::Mov).base_disp(Register::Rbp, -32).reg(Register::Rcx);
    EXPECT(Opcode::Jmp).label(1);
    EXPECT(Opcode::Label).label(3);
    EXPECT(Opcode::Mov).reg(Register::Rax).base_disp(Register::Rbp, -32);
    EXPECT(Opcode::Pop).reg(Register::Rbp);
    EXPECT(Opcode::Ret);
    RUN(expected);
}

} // namespace

} // namespace bamf::x86
