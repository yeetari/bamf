#pragma once

#include <bamf/ir/Expression.hh>

namespace bamf {

template <typename T>
class ConstExpr : public Expression {
    const T m_value;

public:
    constexpr explicit ConstExpr(T value) : m_value(value) {}

    void dump() const override;

    T value() const { return m_value; }
};

} // namespace bamf
