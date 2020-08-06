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
    std::unordered_map<const Value *, std::size_t> value_map;
    auto versioned_value = [&](const Value *value) {
        if (!value_map.contains(value)) {
            value_map.emplace(value, value_map.size());
        }
        return value_map[value];
    };

    auto printable_value = [&](Value *value) {
        if (auto *constant = value->as<Constant<std::size_t>>()) {
            return std::to_string(constant->value());
        }
        if (value->has_name()) {
            return '%' + value->name();
        }
        return '%' + std::to_string(versioned_value(value));
    };

    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (inst->has_name() || (!inst->is<StoreInst>() && !inst->is<RetInst>())) {
                std::cout << printable_value(inst.get()) << " = ";
            }

            if (auto *alloc = inst->as<AllocInst>()) {
                std::cout << "alloc";
            } else if (auto *load = inst->as<LoadInst>()) {
                std::cout << "load " << printable_value(load->ptr());
            } else if (auto *store = inst->as<StoreInst>()) {
                std::cout << "store " << printable_value(store->dst());
                std::cout << ", " << printable_value(store->src());
            } else if (auto *ret = inst->as<RetInst>()) {
                std::cout << "ret " << printable_value(ret->ret_val());
            }
            std::cout << '\n';
        }
    }
}

} // namespace bamf
