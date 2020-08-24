#pragma once

#include <bamf/support/Logger.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <string>
#include <utility>

namespace bamf {

class Function;
class PassManager;
class Program;

class Pass {
    const std::string m_name;

protected:
    PassManager *const m_manager;
    const Logger m_logger;

    Pass(PassManager *manager, std::string name) : m_manager(manager), m_name(std::move(name)), m_logger(m_name) {}

public:
    BAMF_MAKE_NON_COPYABLE(Pass)
    BAMF_MAKE_NON_MOVABLE(Pass)
    virtual ~Pass() = default;

    virtual void run_on(Program *program) {}
    virtual void run_on(Function *function) {}

    const std::string &name() const { return m_name; }
};

} // namespace bamf
