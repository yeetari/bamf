#include <bamf/core/DecompilationContext.hh>
#include <bamf/core/Executable.hh>
#include <bamf/core/InputFile.hh>
#include <bamf/debug/Verifier.hh>
#include <bamf/frontend/RegisterLocaliser.hh>
#include <bamf/frontend/StackSimulator.hh>
#include <bamf/pass/PassManager.hh>
#include <bamf/support/Logger.hh>
#include <bamf/support/Stream.hh>
#include <bamf/transforms/AllocPromoter.hh>
#include <bamf/transforms/CfgSimplifier.hh>
#include <bamf/transforms/ConstantBranchEvaluator.hh>
#include <bamf/transforms/ConstantFolder.hh>
#include <bamf/transforms/TriviallyDeadInstPruner.hh>
#include <bamf/x86/Decoder.hh>
#include <bamf/x86/Frontend.hh>
#include <bamf/x86/MachineInst.hh>

#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

using namespace bamf;

int main(int argc, char **argv) {
    Logger logger("bamf-perm");
    if (argc != 2) {
        logger.info("Usage: {} <file>", argv[0]);
        return 1;
    }

    logger.debug("Loading {} into memory", argv[1]);
    InputFile file(argv[1]);
    Executable executable = load_executable(file);

    logger.debug("Decoding {}", argv[1]);
    Stream stream(executable.code, executable.code_size);
    x86::Decoder decoder(&stream);
    std::vector<x86::MachineInst> insts;
    while (decoder.has_next()) {
        insts.push_back(decoder.next_inst());
    }

    std::array<std::string, 8> array = {
        "alloc-promoter",     "cfg-simplifier",  "constant-branch-evaluator",  "constant-folder",
        "register-localiser", "stack-simulator", "trivially-dead-inst-pruner", "verifier"};
    std::sort(array.begin(), array.end());
    int total_permutations = 1;
    for (int i = 1; i <= array.size(); i++) {
        total_permutations *= i;
    }
    logger.info("Running {} total permutations", total_permutations);

    for (int i = 1; std::next_permutation(array.begin(), array.end()); i++) {
        logger.warn("{}/{}", i, total_permutations);
        DecompilationContext decomp_ctx;
        PassManager pass_manager;
        for (const auto &str : array) {
            if (str == "alloc-promoter") {
                pass_manager.add<AllocPromoter>();
            } else if (str == "cfg-simplifier") {
                pass_manager.add<CfgSimplifier>();
            } else if (str == "constant-branch-evaluator") {
                pass_manager.add<ConstantBranchEvaluator>();
            } else if (str == "constant-folder") {
                pass_manager.add<ConstantFolder>();
            } else if (str == "register-localiser") {
                pass_manager.add<RegisterLocaliser>(decomp_ctx);
            } else if (str == "stack-simulator") {
                pass_manager.add<StackSimulator>(decomp_ctx);
            } else if (str == "trivially-dead-inst-pruner") {
                pass_manager.add<TriviallyDeadInstPruner>();
            } else if (str == "verifier") {
                pass_manager.add<Verifier>();
            } else {
                assert(false);
            }
        }
        auto program = x86::Frontend(insts, &decomp_ctx).run();
        pass_manager.run(program.get());
    }
}
