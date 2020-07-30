#include <bamf/transforms/SsaTranslator.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Statement.hh>
#include <bamf/ir/Statements.hh>

namespace bamf {

void SsaTranslator::run_on(BasicBlock *block) {
    auto update_version = [&](Local *local) {
        if (local == nullptr) {
            return;
        }
        std::size_t new_version = m_stacks[local->name()].back();
        local->set_version(new_version);
    };

    for (const auto &stmt : *block) {
        if (auto *assign = stmt->as<AssignStmt>()) {
            auto *expr = assign->expr();
            update_version(expr->as<Local>());
        } else if (auto *ret = stmt->as<RetStmt>()) {
            update_version(ret->local());
        }

        if (auto *assign = stmt->as<AssignStmt>()) {
            const auto &dst_name = assign->dst()->name();
            std::size_t counter = m_counters[dst_name];
            m_stacks[dst_name].push_back(counter);
            m_counters[dst_name] = counter + 1;
            assign->dst()->set_version(counter);
        }
    }
}

void SsaTranslator::run_on(Function *function) {
    m_counters.clear();
    m_stacks.clear();
    run_on(function->entry());
}

} // namespace bamf
