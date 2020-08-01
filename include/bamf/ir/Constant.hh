#pragma once

#include <bamf/ir/Value.hh>

namespace bamf {

template <typename T>
class Constant : public Value {
    const T m_value;

public:
    constexpr explicit Constant(T value) : m_value(value) {}

    T value() const { return m_value; }
};

} // namespace bamf
