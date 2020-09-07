#include <bamf/core/DecompilationContext.hh>
#include <bamf/core/Executable.hh>
#include <bamf/core/InputFile.hh>
#include <bamf/debug/Dumper.hh>
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
#include <bamf/x86/Backend.hh>
#include <bamf/x86/BackendResult.hh>
#include <bamf/x86/Decoder.hh>
#include <bamf/x86/Frontend.hh>

#include <iostream>
#include <vector>

using namespace bamf;

int main(int argc, char **argv) {
    Logger logger("bamf-decomp");
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

    DecompilationContext decomp_ctx;
    auto program = x86::Frontend(insts, &decomp_ctx).run();
    PassManager pass_manager;
    pass_manager.add<Verifier>();
    pass_manager.add<StackSimulator>(decomp_ctx);
    pass_manager.add<RegisterLocaliser>(decomp_ctx);
    pass_manager.add<AllocPromoter>();
    pass_manager.add<TriviallyDeadInstPruner>();
    pass_manager.add<ConstantBranchEvaluator>();
    pass_manager.add<ConstantFolder>();
//    pass_manager.add<CfgSimplifier>();
    pass_manager.add<Verifier>();
    pass_manager.add<Dumper>();
    pass_manager.add<x86::Backend>();
    pass_manager.run(program.get());

    std::cout << "bits 64\n";
    for (auto &function : *program) {
        std::cout << "global " << function->name() << '\n';
        std::cout << function->name() << ":\n";
        auto *result = pass_manager.get<x86::BackendResult>(function.get());
        for (auto &inst : *result) {
            std::cout << x86::dump_inst(inst, false) << '\n';
        }
    }
}
