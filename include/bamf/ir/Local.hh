#pragma once

#include <bamf/ir/Expression.hh>

#include <cstdint>
#include <string>
#include <utility>

namespace bamf {

class Local : public Expression {
    const std::string m_name;
    std::size_t m_version{0};

public:
    explicit Local(std::string name) : m_name(std::move(name)) {}

    void dump() const override;
    void set_version(std::size_t version) { m_version = version; }

    const std::string &name() const { return m_name; }
    std::size_t version() const { return m_version; }
};

} // namespace bamf
