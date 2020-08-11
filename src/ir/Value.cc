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

void Value::add_user(Value *user) {
    m_users.push_back(user);
}

void Value::remove_user(Value *user) {
    auto it = std::find(m_users.begin(), m_users.end(), user);
    if (it != m_users.end()) {
        m_users.erase(it);
    }
}

void Value::replace_all_uses_with(Value *value) {
    for (auto **use : m_uses) {
        *use = value;
        if (value != nullptr) {
            value->add_use(use);
        }
    }
    for (auto *user : m_users) {
        if (value != nullptr) {
            value->add_user(user);
        }
    }
    m_uses.clear();
    m_users.clear();
}

} // namespace bamf
