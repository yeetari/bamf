#include <bamf/ir/Local.hh>

#include <iostream>

namespace bamf {

void Local::dump() const {
    std::cout << "%" << m_name << '_' << m_version;
}

} // namespace bamf
