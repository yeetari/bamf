#include <bamf/pass/PassManager.hh>

#include <iostream>

namespace bamf {

void PassManager::run(Function *function) const {
    for (const auto &pass : m_passes) {
        std::cerr << "Running " << pass->name() << '\n';
        pass->run_on(function);
    }
}

} // namespace bamf
