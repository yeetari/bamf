#include <bamf/ir/Statements.hh>

#include <iostream>

namespace bamf {

void AssignStmt::dump() const {
    m_dst.dump();
    std::cout << " = ";
    m_expr->dump();
    std::cout << '\n';
}

void RetStmt::dump() const {
    std::cout << "ret ";
    m_local.dump();
    std::cout << '\n';
}

} // namespace bamf
