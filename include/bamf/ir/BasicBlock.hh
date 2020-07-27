#pragma once

#include <bamf/ir/Statement.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace bamf {

class BasicBlock {
    std::vector<std::unique_ptr<Statement>> m_statements;

public:
    BAMF_MAKE_ITERABLE(m_statements)
    BAMF_MAKE_NON_COPYABLE(BasicBlock)
    BAMF_MAKE_NON_MOVABLE(BasicBlock)

    BasicBlock() = default;
    ~BasicBlock() = default;

    void dump() const;

    template <typename Stmt, typename... Args>
    Stmt *insert(Args &&... args) requires std::derived_from<Stmt, Statement> {
        return static_cast<Stmt *>(m_statements.emplace_back(new Stmt(std::forward<Args>(args)...)).get());
    }
};

} // namespace bamf
