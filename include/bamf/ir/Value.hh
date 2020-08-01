#pragma once

#include <string>
#include <utility>

namespace bamf {

class Value {
    std::string m_name;

public:
    virtual ~Value() = default;

    template <typename T>
    T *as();

    template <typename T>
    bool is();

    void set_name(std::string name) { m_name = std::move(name); }
    bool has_name() const { return !m_name.empty(); }

    const std::string &name() const { return m_name; }
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
