#pragma once

#include <bamf/ir/Program.hh>
#include <bamf/x86/MachineInst.hh>
#include <bamf/x86/Register.hh>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace bamf {

class BasicBlock;
class DecompilationContext;
class Function;
struct GlobalVariable;
class Value;

} // namespace bamf

namespace bamf::x86 {

struct Operand;

class Frontend {
    const std::vector<MachineInst> &m_insts;
    DecompilationContext *const m_decomp_ctx;
    Program *m_program{nullptr};
    Function *m_function{nullptr};
    BasicBlock *m_block{nullptr};

    std::unordered_map<std::uintptr_t, BasicBlock *> m_blocks;
    std::unordered_map<Register, GlobalVariable *> m_phys_regs;
    GlobalVariable *m_of{nullptr};
    GlobalVariable *m_sf{nullptr};
    GlobalVariable *m_zf{nullptr};

    Value *phys_dst(Register);
    Value *phys_src(Register);
    Value *load_op(const Operand &src_op);
    void store_op(const Operand &dst_op, Value *val);

    void translate_add(const Operand &lhs, const Operand &rhs);
    void translate_cmp(const Operand &lhs, const Operand &rhs);
    void translate_inc(const Operand &dst);
    void translate_jg(const Operand &target, BasicBlock *false_block);
    void translate_jge(const Operand &target, BasicBlock *false_block);
    void translate_jle(const Operand &target, BasicBlock *false_block);
    void translate_jmp(const Operand &target);
    void translate_mov(const Operand &dst, const Operand &src);
    void translate_pop(const Operand &dst);
    void translate_push(const Operand &src);
    void translate_ret();
    void translate_shl(const Operand &dst, const Operand &src);

    void build_jump_targets();
    void build_registers();

public:
    Frontend(const std::vector<MachineInst> &insts, DecompilationContext *decomp_ctx)
        : m_insts(insts), m_decomp_ctx(decomp_ctx) {}

    std::unique_ptr<Program> run();
};

} // namespace bamf::x86
