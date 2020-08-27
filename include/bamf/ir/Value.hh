#pragma once

#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <string>
#include <utility>
#include <vector>

namespace bamf {

class Value {
    std::string m_name;
    std::vector<Value *> m_users;

protected:
    Value() = default;

public:
    BAMF_MAKE_NON_COPYABLE(Value)
    BAMF_MAKE_NON_MOVABLE(Value)

    virtual ~Value() = default;

    template <typename T>
    T *as();

    template <typename T>
    bool is();

    void add_user(Value *user);
    void remove_user(Value *user);
    void replace_all_uses_with(Value *value);
    virtual void replace_uses_of_with(Value *orig, Value *repl);

    void set_name(std::string name) { m_name = std::move(name); }
    bool has_name() const { return !m_name.empty(); }

    const std::string &name() const { return m_name; }
    const std::vector<Value *> &users() const { return m_users; }
};

template <typename T>
T *Value::as() {
    return dynamic_cast<T *>(this);
}

template <typename T>
bool Value::is() {
    return as<T>() != nullptr;
}

} // namespace bamf
