add_library(bamf
    io/ElfLoader.cc
    io/InputFile.cc
    ir/BasicBlock.cc
    ir/Expressions.cc
    ir/Local.cc
    ir/Statements.cc
    support/Stream.cc
    x86/Decoder.cc
    x86/MachineInst.cc
    x86/Register.cc)
target_compile_features(bamf PUBLIC cxx_std_20)
target_include_directories(bamf PUBLIC ${PROJECT_SOURCE_DIR}/include)
