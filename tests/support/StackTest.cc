#include <bamf/support/Stack.hh>

#include <gtest/gtest.h>

namespace bamf {

namespace {

TEST(StackTest, PushPeekPop) {
    auto *elem_one = new int(5);
    auto *elem_two = new int(6);

    Stack<int> stack;
    stack.push(elem_one);
    stack.push(elem_two);
    EXPECT_EQ(stack.size(), 2);
    EXPECT_FALSE(stack.empty());

    EXPECT_EQ(stack.peek(), elem_two);
    EXPECT_EQ(stack.pop(), elem_two);
    EXPECT_EQ(stack.size(), 1);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.peek(), elem_one);
    EXPECT_EQ(stack.pop(), elem_one);
    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
}

} // namespace

} // namespace bamf
