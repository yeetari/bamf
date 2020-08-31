#pragma once

// TODO: Forward declare
#include <bamf/backend/BackendInstructions.hh>
#include <bamf/ir/InstVisitor.hh>

namespace bamf {

struct BackendInstVisitor : public InstVisitor {
    virtual void visit(MoveInst *) = 0;
};

} // namespace bamf
