#include <bamf/ir/Value.hh>

#include <gtest/gtest.h>

namespace bamf {

namespace {

class TestUser : public Value {
    Value *m_a;
    Value *m_b;

public:
    TestUser(Value *a, Value *b) : m_a(a), m_b(b) {
        m_a->add_user(this);
        m_b->add_user(this);
    }

    ~TestUser() override {
        m_a->remove_user(this);
        m_b->remove_user(this);
    }

    void replace_uses_of_with(Value *orig, Value *repl) override {
        m_a = m_a == orig ? repl : m_a;
        m_b = m_b == orig ? repl : m_b;
    }

    Value *a() const { return m_a; }
    Value *b() const { return m_b; }
};

struct TestValue : public Value {
    TestValue() = default;
};

TEST(ValueTest, Cast) {
    TestValue a;
    TestValue b;
    TestUser c(&a, &b);
    EXPECT_TRUE(a.is<TestValue>());
    EXPECT_TRUE(b.is<TestValue>());
    EXPECT_TRUE(c.is<TestUser>());

    auto *user = c.as<TestUser>();
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->a(), &a);
    EXPECT_EQ(user->b(), &b);
}

TEST(ValueTest, Name) {
    TestValue a;
    EXPECT_FALSE(a.has_name());
    a.set_name("TestValue");
    EXPECT_TRUE(a.has_name());
    EXPECT_EQ(a.name(), "TestValue");
}

TEST(ValueTest, ReplaceAllUsesWith) {
    TestValue a;
    TestValue b;
    TestValue c;
    TestUser user(&a, &b);
    ASSERT_EQ(a.users().size(), 1);
    EXPECT_EQ(a.users()[0], &user);
    ASSERT_EQ(b.users().size(), 1);
    EXPECT_EQ(b.users()[0], &user);
    EXPECT_TRUE(c.users().empty());
    EXPECT_TRUE(user.users().empty());

    a.replace_all_uses_with(&c);
    EXPECT_TRUE(a.users().empty());
    ASSERT_EQ(b.users().size(), 1);
    EXPECT_EQ(b.users()[0], &user);
    ASSERT_EQ(c.users().size(), 1);
    EXPECT_EQ(c.users()[0], &user);
    EXPECT_TRUE(user.users().empty());
}

} // namespace

} // namespace bamf

