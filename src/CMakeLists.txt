add_library(bamf
    core/ElfLoader.cc
    core/InputFile.cc
    ir/BasicBlock.cc
    ir/Expressions.cc
    ir/Function.cc
    ir/Local.cc
    ir/Statements.cc
    pass/PassManager.cc
    support/Stream.cc
    transforms/SsaTranslator.cc
    x86/Decoder.cc
    x86/Frontend.cc
    x86/MachineInst.cc
    x86/Register.cc)
target_compile_features(bamf PUBLIC cxx_std_20)
target_include_directories(bamf PUBLIC ${PROJECT_SOURCE_DIR}/include)
