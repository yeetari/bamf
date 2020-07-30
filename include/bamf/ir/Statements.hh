#pragma once

#include <bamf/ir/Expression.hh>
#include <bamf/ir/Local.hh>
#include <bamf/ir/Statement.hh>

#include <utility>

namespace bamf {

class AssignStmt : public Statement {
    Local m_dst;
    Expression *const m_expr;

public:
    AssignStmt(Local dst, Expression *expr) : m_dst(std::move(dst)), m_expr(expr) {}

    void dump() const override;

    Local *dst() { return &m_dst; }
    Expression *expr() const { return m_expr; }
};

class RetStmt : public Statement {
    Local m_local;

public:
    explicit RetStmt(Local local) : m_local(std::move(local)) {}

    void dump() const override;

    Local *local() { return &m_local; }
};

} // namespace bamf
