#include <bamf/transforms/Dumper.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>

#include <cstdint>
#include <iostream>

namespace bamf {

void Dumper::run_on(Function *function) {
    m_logger.info("Dumping function {}", function->name());
    std::unordered_map<const BasicBlock *, std::size_t> block_map;
    std::unordered_map<const Value *, std::size_t> value_map;
    auto versioned_block = [&](const BasicBlock *block) {
        if (!block_map.contains(block)) {
            block_map.emplace(block, block_map.size());
        }
        return block_map[block];
    };
    auto versioned_value = [&](const Value *value) {
        if (!value_map.contains(value)) {
            value_map.emplace(value, value_map.size());
        }
        return value_map[value];
    };

    auto printable_block = [&](BasicBlock *block) {
        return 'L' + std::to_string(versioned_block(block));
    };
    auto printable_value = [&](Value *value) {
        if (auto *constant = value->as<Constant>()) {
            return std::to_string(constant->value());
        }
        if (value->has_name()) {
            return '%' + value->name();
        }
        return '%' + std::to_string(versioned_value(value));
    };

    for (auto &block : *function) {
        std::cout << printable_block(block.get()) << ":\n";
        for (auto &inst : *block) {
            std::cout << "  ";

            if (auto *branch = inst->as<BranchInst>()) {
                std::cout << "br " << printable_block(branch->dst());
            } else if (auto *cond_branch = inst->as<CondBranchInst>()) {
                std::cout << "br " << printable_value(cond_branch->cond());
                std::cout << ", " << printable_block(cond_branch->true_dst());
                std::cout << ", " << printable_block(cond_branch->false_dst());
            } else if (auto *store = inst->as<StoreInst>()) {
                std::cout << "store " << printable_value(store->ptr());
                std::cout << ", " << printable_value(store->val());
            } else if (auto *ret = inst->as<RetInst>()) {
                std::cout << "ret " << printable_value(ret->ret_val());
            } else {
                std::cout << printable_value(inst.get()) << " = ";
            }

            if (auto *alloc = inst->as<AllocInst>()) {
                std::cout << "alloc";
            } else if (auto *binary = inst->as<BinaryInst>()) {
                switch (binary->op()) {
                case BinaryOp::Add:
                    std::cout << "add ";
                    break;
                case BinaryOp::And:
                    std::cout << "and ";
                    break;
                case BinaryOp::Or:
                    std::cout << "or ";
                    break;
                case BinaryOp::Shl:
                    std::cout << "shl ";
                    break;
                case BinaryOp::Sub:
                    std::cout << "sub ";
                    break;
                case BinaryOp::Xor:
                    std::cout << "xor ";
                    break;
                }
                std::cout << printable_value(binary->lhs());
                std::cout << ", " << printable_value(binary->rhs());
            } else if (auto *compare = inst->as<CompareInst>()) {
                std::cout << "cmp ";
                switch (compare->pred()) {
                case ComparePred::Eq:
                    std::cout << "eq ";
                    break;
                case ComparePred::Ne:
                    std::cout << "ne ";
                    break;
                case ComparePred::Slt:
                    std::cout << "slt ";
                    break;
                }
                std::cout << printable_value(compare->lhs());
                std::cout << ", " << printable_value(compare->rhs());
            } else if (auto *load = inst->as<LoadInst>()) {
                std::cout << "load " << printable_value(load->ptr());
            } else if (auto *phi = inst->as<PhiInst>()) {
                std::cout << "phi (";
                for (bool first = true; auto [block, value] : *phi) {
                    if (!first) {
                        std::cout << ", ";
                    }
                    first = false;
                    std::cout << printable_block(block) << ": ";
                    std::cout << printable_value(value);
                }
                std::cout << ")";
            }
            std::cout << '\n';
        }
    }
}

} // namespace bamf
