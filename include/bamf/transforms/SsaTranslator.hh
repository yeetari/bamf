#pragma once

#include <bamf/pass/Pass.hh>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace bamf {

class BasicBlock;

class SsaTranslator : public Pass {
    std::unordered_map<std::string, std::size_t> m_counters;
    std::unordered_map<std::string, std::vector<std::size_t>> m_stacks;

    void run_on(BasicBlock *block);

public:
    SsaTranslator() : Pass("SSA Translator") {}

    void run_on(Function *function) override;
};

} // namespace bamf
