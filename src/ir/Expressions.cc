#include <bamf/ir/Expressions.hh>

#include <cstdint>
#include <iostream>

namespace bamf {

template <>
void ConstExpr<std::size_t>::dump() const {
    std::cout << m_value;
}

} // namespace bamf
