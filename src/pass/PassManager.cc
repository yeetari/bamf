#include <bamf/pass/PassManager.hh>

namespace bamf {

void PassManager::run(Function *function) const {
    for (const auto &pass : m_passes) {
        m_logger.debug("Running {}", pass->name());
        pass->run_on(function);
    }
}

} // namespace bamf
