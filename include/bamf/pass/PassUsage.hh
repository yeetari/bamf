#pragma once

#include <bamf/pass/PassManager.hh>

#include <utility>
#include <vector>

namespace bamf {

class PassUsage {
    friend PassManager;

private:
    PassManager *const m_manager;
    std::vector<Pass *> m_dependencies;

    explicit PassUsage(PassManager *manager) : m_manager(manager) {}

public:
    template <typename T, typename... Args>
    void depends_on(Args &&... args);
};

template <typename T, typename... Args>
void PassUsage::depends_on(Args &&... args) {
    auto *pass = m_manager->ensure_pass<T>(std::forward<Args>(args)...);
    m_dependencies.push_back(pass);
}

} // namespace bamf
