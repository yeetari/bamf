#include <bamf/ir/Value.hh>

#include <algorithm>

namespace bamf {

void Value::add_use(Value **use) {
    m_uses.push_back(use);
}

void Value::remove_use(Value **use) {
    auto it = std::find(m_uses.begin(), m_uses.end(), use);
    if (it != m_uses.end()) {
        m_uses.erase(it);
    }
}

void Value::replace_all_uses_with(Value *value) {
    for (auto **use : m_uses) {
        *use = value;
        if (value != nullptr) {
            value->add_use(use);
        }
    }
    m_uses.clear();
}

} // namespace bamf
