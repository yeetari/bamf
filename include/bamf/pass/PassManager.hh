#pragma once

#include <bamf/pass/Pass.hh>
#include <bamf/support/Logger.hh>

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace bamf {

class PassUsage;
class Program;

class PassManager {
    friend PassUsage;

private:
    Logger m_logger;
    std::vector<Pass *> m_explicit;
    std::vector<std::unique_ptr<Pass>> m_passes;
    std::unordered_map<std::type_index, Pass *> m_pass_map;

    template <typename T, typename... Args>
    Pass *ensure_pass(Args &&... args);

public:
    PassManager() : m_logger("pass-manager") {}

    template <typename T, typename... Args>
    void add(Args &&... args) requires std::derived_from<T, Pass> {
        m_explicit.push_back(ensure_pass<T>(std::forward<Args>(args)...));
    }

    void run(Program *program) const;
};

template <typename T, typename... Args>
Pass *PassManager::ensure_pass(Args &&... args) {
    auto type = std::type_index(typeid(T));
    if (!m_pass_map.contains(type)) {
        auto *pass = new T(this, std::forward<Args>(args)...);
        m_passes.emplace_back(pass);
        m_pass_map.emplace(type, pass);
    }
    return m_pass_map.at(type);
}

} // namespace bamf
