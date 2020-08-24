#include <bamf/pass/PassManager.hh>

#include <bamf/ir/Program.hh>
#include <bamf/pass/PassUsage.hh>

namespace bamf {

void PassManager::run(Program *program) {
    for (auto *pass : m_explicit) {
        PassUsage usage(this);
        pass->build_usage(&usage);
        for (auto *dep : usage.m_dependencies) {
            dep->run_on(program);
        }

        m_logger.debug("Running {}", pass->name());
        pass->run_on(program);
        for (auto &function : *program) {
            for (auto *dep : usage.m_dependencies) {
                dep->run_on(function.get());
            }
            pass->run_on(function.get());
        }
    }
}

} // namespace bamf
