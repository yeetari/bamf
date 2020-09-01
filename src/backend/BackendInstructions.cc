#include <bamf/backend/BackendInstructions.hh>

#include <bamf/backend/BackendInstVisitor.hh>

namespace bamf {

void MoveInst::accept(InstVisitor *visitor) {
    if (auto *backend_visitor = dynamic_cast<BackendInstVisitor *>(visitor)) {
        backend_visitor->visit(this);
    }
}

void MoveInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_dst = m_dst == orig ? repl : m_dst;
    m_val = m_val == orig ? repl : m_val;
}

} // namespace bamf
