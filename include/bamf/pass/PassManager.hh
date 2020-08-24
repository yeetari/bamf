#pragma once

#include <bamf/pass/Pass.hh>
#include <bamf/support/Logger.hh>

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace bamf {

class Program;

class PassManager {
    Logger m_logger;
    std::vector<std::unique_ptr<Pass>> m_passes;

public:
    PassManager() : m_logger("pass-manager") {}

    template <typename T, typename... Args>
    void add(Args &&... args) requires std::derived_from<T, Pass> {
        m_passes.emplace_back(new T(this, std::forward<Args>(args)...));
    }

    void run(Program *program) const;
};

} // namespace bamf
