#pragma once

// TODO: Forward declare
#include <bamf/ir/Instructions.hh>

namespace bamf {

struct InstVisitor {
    virtual void visit(AllocInst *) = 0;
    virtual void visit(BinaryInst *) = 0;
    virtual void visit(BranchInst *) = 0;
    virtual void visit(CompareInst *) = 0;
    virtual void visit(CondBranchInst *) = 0;
    virtual void visit(LoadInst *) = 0;
    virtual void visit(PhiInst *) = 0;
    virtual void visit(StoreInst *) = 0;
    virtual void visit(RetInst *) = 0;
};

} // namespace bamf
