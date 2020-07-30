#pragma once

#include <string>
#include <utility>

namespace bamf {

class Function;

class Pass {
    const std::string m_name;

protected:
    explicit Pass(std::string name) : m_name(std::move(name)) {}

public:
    virtual void run_on(Function *function) = 0;

    const std::string &name() const { return m_name; }
};

} // namespace bamf
