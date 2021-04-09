add_library(bamf
    analyses/ControlFlowAnalyser.cc
    analyses/ControlFlowAnalysis.cc
    analyses/DataFlowAnalyser.cc
    backend/BackendInstructions.cc
    backend/MoveInserter.cc
    backend/RegAllocator.cc
    core/DecompilationContext.cc
    core/ElfLoader.cc
    core/Executable.cc
    core/InputFile.cc
    debug/Dumper.cc
    debug/Verifier.cc
    frontend/RegisterLocaliser.cc
    frontend/StackSimulator.cc
    ir/BasicBlock.cc
    ir/Function.cc
    ir/Instruction.cc
    ir/Instructions.cc
    ir/Program.cc
    ir/Value.cc
    pass/PassManager.cc
    pass/Statistic.cc
    support/Stream.cc
    transforms/AllocPromoter.cc
    transforms/CfgSimplifier.cc
    transforms/ConstantBranchEvaluator.cc
    transforms/ConstantFolder.cc
    transforms/TriviallyDeadInstPruner.cc
    x86/Backend.cc
    x86/Builder.cc
    x86/Decoder.cc
    x86/Frontend.cc
    x86/MachineInst.cc
    x86/Register.cc)
target_compile_features(bamf PUBLIC cxx_std_20)
target_include_directories(bamf PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bamf PRIVATE fmt::fmt)
