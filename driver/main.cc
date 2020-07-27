#include <bamf/core/Executable.hh>
#include <bamf/io/ElfLoader.hh>
#include <bamf/io/InputFile.hh>
#include <bamf/support/Stream.hh>
#include <bamf/x86/Decoder.hh>
#include <bamf/x86/Frontend.hh>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace bamf;

namespace {

void print_usage(const char *prog_name) {
    std::cerr << "Usage: " << prog_name << " [options] <file>\n";
}

} // namespace

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string file_name;
    std::string file_type;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.starts_with("-f")) {
            file_type = arg.substr(2);
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
    std::cerr << "Decoding " << file_name << " as " << file_type << (file_type_detected ? " (detected)\n" : "\n");
    if (file_type == "bin") {
        executable.code = file.get<char>(0);
        executable.code_size = file.size();
    } else if (file_type == "elf") {
        load_elf_exec(file, &executable);
    } else {
        throw std::runtime_error("Unknown file type " + file_type);
    }

    std::cerr << std::hex;
    for (std::size_t i = 0; i < executable.code_size; i++) {
        std::cerr << (static_cast<int>(executable.code[i]) & 0xFF) << ' ';
    }
    std::cerr << '\n' << std::dec;

    Stream stream(executable.code, executable.code_size);
    x86::Decoder decoder(&stream);
    x86::Frontend frontend(&decoder);
    auto block = frontend.run();
    block->dump();
}
