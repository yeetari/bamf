#pragma once

#include <string>
#include <utility>
#include <vector>

namespace bamf {

class Value {
    std::string m_name;
    std::vector<Value **> m_uses;

public:
    virtual ~Value() = default;

    template <typename T>
    T *as();

    template <typename T>
    bool is();

    void add_use(Value **use);
    void remove_use(Value **use);
    void replace_all_uses_with(Value *value);

    void set_name(std::string name) { m_name = std::move(name); }
    bool has_name() const { return !m_name.empty(); }

    const std::string &name() const { return m_name; }
    const std::vector<Value **> &uses() const { return m_uses; }
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
