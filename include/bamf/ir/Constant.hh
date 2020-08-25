#pragma once

#include <bamf/ir/Value.hh>

#include <cstdint>

namespace bamf {

class Constant : public Value {
    const std::uint64_t m_value;

public:
    explicit Constant(std::uint64_t value) : m_value(value) {}

    std::uint64_t value() const { return m_value; }
};

} // namespace bamf
