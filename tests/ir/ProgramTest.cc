#include <bamf/ir/Program.hh>

#include <gtest/gtest.h>

namespace bamf {

namespace {

TEST(ProgramTest, AddRemoveGlobal) {
    Program program;
    auto *one = program.add_global();
    auto *two = program.add_global();
    one->set_name("g_var1");
    two->set_name("g_var2");
    EXPECT_EQ(program.globals().size(), 2);
    program.remove_global(one);
    ASSERT_EQ(program.globals().size(), 1);
    EXPECT_EQ(program.globals()[0].get(), two);
    EXPECT_EQ(program.globals()[0]->name(), "g_var2");
    program.remove_global(two);
    EXPECT_TRUE(program.globals().empty());
}

} // namespace

} // namespace bamf


