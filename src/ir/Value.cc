#include <bamf/ir/Value.hh>

namespace bamf {

void Value::add_use(Value **use) {
    m_uses.push_back(use);
}

void Value::remove_use(Value **use) {
    m_uses.remove(use);
}

void Value::replace_all_uses_with(Value *value) {
    for (auto **use : m_uses) {
        *use = value;
        value->add_use(use);
    }
    m_uses.clear();
}

} // namespace bamf

