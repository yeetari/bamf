#pragma once

#include <bamf/ir/Expression.hh>
#include <bamf/ir/Local.hh>
#include <bamf/ir/Statement.hh>

namespace bamf {

class AssignStmt : public Statement {
    const Local *const m_dst;
    const Expression *const m_expr;

public:
    constexpr AssignStmt(const Local *dst, const Expression *expr) : m_dst(dst), m_expr(expr) {}

    void dump() const override;

    const Local *dst() const { return m_dst; }
    const Expression *expr() const { return m_expr; }
};

class RetStmt : public Statement {
    const Local *const m_local;

public:
    constexpr explicit RetStmt(const Local *local) : m_local(local) {}

    void dump() const override;

    const Local *local() const { return m_local; }
};

} // namespace bamf
