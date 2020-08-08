#include <bamf/pass/PassManager.hh>

#include <bamf/ir/Program.hh>

namespace bamf {

void PassManager::run(Program *program) const {
    for (const auto &pass : m_passes) {
        m_logger.debug("Running {}", pass->name());
        pass->run_on(program);
        for (auto &function : *program) {
            pass->run_on(function.get());
        }
    }
}

} // namespace bamf
