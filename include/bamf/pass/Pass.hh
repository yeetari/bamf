#pragma once

#include <bamf/support/Logger.hh>

#include <string>
#include <utility>

namespace bamf {

class Function;

class Pass {
    const std::string m_name;

protected:
    Logger m_logger;
    explicit Pass(std::string name) : m_name(std::move(name)), m_logger(m_name) {}

public:
    virtual ~Pass() = default;
    virtual void run_on(Function *function) = 0;

    const std::string &name() const { return m_name; }
};

} // namespace bamf
