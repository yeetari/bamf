#pragma once

#include <bamf/ir/Value.hh>

#include <cstdint>

namespace bamf {

class Constant : public Value {
    const std::size_t m_value;

public:
    explicit Constant(std::size_t value) : m_value(value) {}

    std::size_t value() const { return m_value; }
};

} // namespace bamf
