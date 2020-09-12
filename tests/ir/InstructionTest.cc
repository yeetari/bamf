#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Instruction.hh>

#include <gtest/gtest.h>

namespace bamf {

namespace {

class TestInstruction : public Instruction {
    void accept(InstVisitor *) override {}
};

TEST(InstructionTest, Parent) {
    BasicBlock block;
    TestInstruction instruction;
    EXPECT_FALSE(instruction.has_parent());
    instruction.set_parent(&block);
    EXPECT_TRUE(instruction.has_parent());
    EXPECT_EQ(instruction.parent(), &block);
}

} // namespace

} // namespace bamf

