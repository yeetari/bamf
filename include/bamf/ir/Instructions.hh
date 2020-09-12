#pragma once

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
    BinaryInst(BinaryOp op, Value *lhs, Value *rhs);
    ~BinaryInst() override;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    BinaryOp op() const { return m_op; }
    Value *lhs() const { return m_lhs; }
    Value *rhs() const { return m_rhs; }
};

class BranchInst : public Instruction {
    BasicBlock *m_dst;

public:
    explicit BranchInst(BasicBlock *dst);
    ~BranchInst() override;

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
    CompareInst(ComparePred pred, Value *lhs, Value *rhs);
    ~CompareInst() override;

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
    CondBranchInst(Value *cond, BasicBlock *false_dst, BasicBlock *true_dst);
    ~CondBranchInst() override;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *cond() const { return m_cond; }
    BasicBlock *false_dst() const { return m_false_dst; }
    BasicBlock *true_dst() const { return m_true_dst; }
};

class LoadInst : public Instruction {
    Value *m_ptr;

public:
    explicit LoadInst(Value *ptr);
    ~LoadInst() override;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ptr() const { return m_ptr; }
};

class PhiInst : public Instruction {
    std::unordered_map<BasicBlock *, Value *> m_incoming;

public:
    BAMF_MAKE_ITERABLE(m_incoming)

    PhiInst() = default;
    ~PhiInst() override;

    void add_incoming(BasicBlock *block, Value *value);
    void remove_incoming(BasicBlock *block);

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    const std::unordered_map<BasicBlock *, Value *> &incoming() const { return m_incoming; }
};

class StoreInst : public Instruction {
    Value *m_ptr;
    Value *m_val;

public:
    StoreInst(Value *ptr, Value *val);
    ~StoreInst() override;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ptr() const { return m_ptr; }
    Value *val() const { return m_val; }
};

class RetInst : public Instruction {
    Value *m_ret_val;

public:
    explicit RetInst(Value *ret_val);
    ~RetInst() override;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ret_val() const { return m_ret_val; }
};

} // namespace bamf
