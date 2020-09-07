#include <bamf/core/Executable.hh>
#include <bamf/core/InputFile.hh>
#include <bamf/support/Logger.hh>
#include <bamf/support/Stream.hh>
#include <bamf/x86/Decoder.hh>
#include <bamf/x86/MachineInst.hh>

#include <iostream>

using namespace bamf;

int main(int argc, char **argv) {
    Logger logger("bamf-disasm");
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
    while (decoder.has_next()) {
        std::cout << x86::dump_inst(decoder.next_inst()) << '\n';
    }
}
