#pragma once

// TODO: Remove BasicBlock include once we move more stuff to cc file
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Value.hh>
#include <bamf/support/Iterable.hh>

#include <unordered_map>

namespace bamf {

struct AllocInst : public Instruction {
    void accept(InstVisitor *visitor) override;
};

enum class BinaryOp {
    Add,
    And,
    Or,
    Shl,
    Sub,
    Xor,
};

class BinaryInst : public Instruction {
    BinaryOp m_op;
    Value *m_lhs;
    Value *m_rhs;

public:
    BinaryInst(BinaryOp op, Value *lhs, Value *rhs) : m_op(op), m_lhs(lhs), m_rhs(rhs) {
        m_lhs->add_user(this);
        m_rhs->add_user(this);
    }

    ~BinaryInst() override {
        m_lhs->remove_user(this);
        m_rhs->remove_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    BinaryOp op() const { return m_op; }
    Value *lhs() const { return m_lhs; }
    Value *rhs() const { return m_rhs; }
};

class BranchInst : public Instruction {
    BasicBlock *m_dst;

public:
    explicit BranchInst(BasicBlock *dst) : m_dst(dst) { m_dst->add_user(this); }
    ~BranchInst() override { m_dst->remove_user(this); }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    BasicBlock *dst() const { return m_dst; }
};

enum class ComparePred {
    Eq,
    Ne,
    Slt,
};

class CompareInst : public Instruction {
    ComparePred m_pred;
    Value *m_lhs;
    Value *m_rhs;

public:
    CompareInst(ComparePred pred, Value *lhs, Value *rhs) : m_pred(pred), m_lhs(lhs), m_rhs(rhs) {
        m_lhs->add_user(this);
        m_rhs->add_user(this);
    }

    ~CompareInst() override {
        m_lhs->remove_user(this);
        m_rhs->remove_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    ComparePred pred() const { return m_pred; }
    Value *lhs() const { return m_lhs; }
    Value *rhs() const { return m_rhs; }
};

class CondBranchInst : public Instruction {
    Value *m_cond;
    BasicBlock *m_false_dst;
    BasicBlock *m_true_dst;

public:
    CondBranchInst(Value *cond, BasicBlock *false_dst, BasicBlock *true_dst)
        : m_cond(cond), m_false_dst(false_dst), m_true_dst(true_dst) {
        m_cond->add_user(this);
        m_false_dst->add_user(this);
        m_true_dst->add_user(this);
    }

    ~CondBranchInst() override {
        m_cond->remove_user(this);
        m_false_dst->remove_user(this);
        m_true_dst->remove_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *cond() const { return m_cond; }
    BasicBlock *false_dst() const { return m_false_dst; }
    BasicBlock *true_dst() const { return m_true_dst; }
};

class LoadInst : public Instruction {
    Value *m_ptr;

public:
    explicit LoadInst(Value *ptr) : m_ptr(ptr) { m_ptr->add_user(this); }
    ~LoadInst() override { m_ptr->remove_user(this); }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ptr() const { return m_ptr; }
};

class PhiInst : public Instruction {
    std::unordered_map<BasicBlock *, Value *> m_incoming;

public:
    BAMF_MAKE_ITERABLE(m_incoming)

    PhiInst() = default;
    ~PhiInst() override {
        for (auto [block, value] : m_incoming) {
            block->remove_user(this);
            value->remove_user(this);
        }
    }

    void add_incoming(BasicBlock *block, Value *value) {
        m_incoming[block] = value;
        block->add_user(this);
        value->add_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    void remove_incoming(BasicBlock *block);
    const std::unordered_map<BasicBlock *, Value *> &incoming() const { return m_incoming; }
};

class StoreInst : public Instruction {
    Value *m_ptr;
    Value *m_val;

public:
    StoreInst(Value *ptr, Value *val) : m_ptr(ptr), m_val(val) {
        m_ptr->add_user(this);
        m_val->add_user(this);
    }

    ~StoreInst() override {
        m_ptr->remove_user(this);
        m_val->remove_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ptr() const { return m_ptr; }
    Value *val() const { return m_val; }
};

class RetInst : public Instruction {
    Value *m_ret_val;

public:
    explicit RetInst(Value *ret_val) : m_ret_val(ret_val) { m_ret_val->add_user(this); }
    ~RetInst() override { m_ret_val->remove_user(this); }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ret_val() const { return m_ret_val; }
};

} // namespace bamf
