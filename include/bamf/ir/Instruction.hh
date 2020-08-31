#pragma once

#include <bamf/ir/Value.hh>

namespace bamf {

class BasicBlock;
class InstVisitor;

class Instruction : public Value {
    BasicBlock *m_parent{nullptr};

public:
    void remove_from_parent();
    virtual void accept(InstVisitor *visitor) = 0;

    void set_parent(BasicBlock *parent) { m_parent = parent; }
    bool has_parent() const { return m_parent != nullptr; }

    BasicBlock *parent() const { return m_parent; }
};

} // namespace bamf
