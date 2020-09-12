#include <bamf/ir/Value.hh>

#include <algorithm>
#include <cassert>

namespace bamf {

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
    for (auto *user : m_users) {
        user->replace_uses_of_with(this, value);
        if (value != nullptr) {
            value->add_user(user);
        }
    }
    // TODO: replace_uses_of_with() should remove itself as user. Replace this with assert(m_users.empty());
    m_users.clear();
}

void Value::replace_uses_of_with(Value *, Value *) {
    assert(false);
}

} // namespace bamf
