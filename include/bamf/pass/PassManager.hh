#pragma once

#include <bamf/pass/Pass.hh>

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace bamf {

class Function;

class PassManager {
    std::vector<std::unique_ptr<Pass>> m_passes;

public:
    template <typename T, typename... Args>
    void add(Args &&... args) requires std::derived_from<T, Pass> {
        m_passes.emplace_back(new T(std::forward<Args>(args)...));
    }

    void run(Function *function) const;
};

} // namespace bamf
