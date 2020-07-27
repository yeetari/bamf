#pragma once

#include <bamf/ir/Expression.hh>

#include <string>
#include <utility>

namespace bamf {

class Local : public Expression {
    std::string m_name;

public:
    explicit Local(std::string name) : m_name(std::move(name)) {}

    void dump() const override;

    const std::string &name() const { return m_name; }
};

} // namespace bamf
