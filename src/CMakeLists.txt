add_library(bamf
    core/DecompilationContext.cc
    core/ElfLoader.cc
    core/InputFile.cc
    ir/BasicBlock.cc
    ir/Instruction.cc
    ir/Value.cc
    pass/PassManager.cc
    support/Stream.cc
    transforms/AllocPromoter.cc
    transforms/ConstantBranchEvaluator.cc
    transforms/Dumper.cc
    transforms/RegisterLocaliser.cc
    transforms/StackSimulator.cc
    transforms/TriviallyDeadInstPruner.cc
    x86/Decoder.cc
    x86/Frontend.cc
    x86/MachineInst.cc
    x86/Register.cc)
target_compile_features(bamf PUBLIC cxx_std_20)
target_include_directories(bamf PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bamf PRIVATE fmt::fmt)
