#include <bamf/backend/BackendInstructions.hh>

#include <bamf/backend/BackendInstVisitor.hh>

namespace bamf {

void MoveInst::accept(InstVisitor *visitor) {
    if (auto *backend_visitor = dynamic_cast<BackendInstVisitor *>(visitor)) {
        backend_visitor->visit(this);
    }
}

} // namespace bamf
