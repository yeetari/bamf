#pragma once

#include <string>
#include <utility>
#include <vector>

namespace bamf {

class Value {
    std::string m_name;

    // TODO: Once we add a `Value::replace_uses_of_with(Value *, Value *)` we can get rid of `m_uses`
    std::vector<Value **> m_uses;
    std::vector<Value *> m_users;

public:
    virtual ~Value() = default;

    template <typename T>
    T *as();

    template <typename T>
    bool is();

    void add_use(Value **use);
    void remove_use(Value **use);
    void add_user(Value *user);
    void remove_user(Value *user);
    void replace_all_uses_with(Value *value);

    void set_name(std::string name) { m_name = std::move(name); }
    bool has_name() const { return !m_name.empty(); }

    const std::string &name() const { return m_name; }
    const std::vector<Value **> &uses() const { return m_uses; }
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
