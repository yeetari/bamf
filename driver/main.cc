#include <bamf/core/DecompilationContext.hh>
#include <bamf/core/ElfLoader.hh>
#include <bamf/core/Executable.hh>
#include <bamf/core/InputFile.hh>
#include <bamf/pass/PassManager.hh>
#include <bamf/support/Logger.hh>
#include <bamf/support/Stream.hh>
#include <bamf/transforms/AllocPromoter.hh>
#include <bamf/transforms/Dumper.hh>
#include <bamf/transforms/RegisterLocaliser.hh>
#include <bamf/transforms/StackSimulator.hh>
#include <bamf/transforms/TriviallyDeadInstPruner.hh>
#include <bamf/x86/Decoder.hh>
#include <bamf/x86/Frontend.hh>

#include <stdexcept>
#include <string>

using namespace bamf;

namespace {

Logger s_logger("driver");

void print_usage(const char *prog_name) {
    s_logger.info("Usage: {} [options] <file>", prog_name);
}

} // namespace

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string file_name;
    std::string file_type;
    std::string mode("decomp");
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.starts_with("-f")) {
            file_type = arg.substr(2);
        } else if (arg.starts_with("-m")) {
            mode = arg.substr(2);
        } else {
            file_name = arg;
            break;
        }
    }

    bool file_type_detected = false;
    if (file_type.empty()) {
        file_type = file_name.substr(file_name.find_last_of('.') + 1);
        file_type_detected = true;
    }

    Executable executable{};
    InputFile file(file_name.c_str());
    s_logger.info("{} {} as {} {}", mode == "disasm" ? "Disassembling" : "Decompiling", file_name, file_type,
                  file_type_detected ? "(detected)" : "");
    if (file_type == "bin") {
        executable.code = file.get<std::uint8_t>(0);
        executable.code_size = file.size();
    } else if (file_type == "elf") {
        load_elf_exec(file, &executable);
    } else {
        throw std::runtime_error("Unknown file type " + file_type);
    }

    Stream stream(executable.code, executable.code_size);
    if (mode == "disasm") {
        x86::Decoder decoder(&stream);
        while (decoder.has_next()) {
            auto inst = decoder.next_inst();
            x86::dump_inst(inst);
        }
    } else if (mode == "decomp") {
        DecompilationContext decomp_ctx;
        x86::Frontend frontend(&stream, &decomp_ctx);
        auto program = frontend.run();
        PassManager pass_manager;
        pass_manager.add<StackSimulator>(decomp_ctx);
        pass_manager.add<RegisterLocaliser>(decomp_ctx);
        pass_manager.add<AllocPromoter>();
        pass_manager.add<TriviallyDeadInstPruner>();
        pass_manager.add<Dumper>();
        pass_manager.run(program.get());
    } else {
        throw std::runtime_error("Invalid mode " + mode + " (valid disasm/decomp)");
    }
}
