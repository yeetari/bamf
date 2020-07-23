#include <bamf/io/InputFile.hh>
#include <bamf/io/InputStream.hh>
#include <bamf/x86/Decoder.hh>

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
    std::string file_type("bin");
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.starts_with("-f")) {
            file_type = arg.substr(2);
        } else {
            file_name = arg;
            break;
        }
    }

    std::cerr << "Decoding " << file_name << " as " << file_type << '\n';

    InputFile file(file_name.c_str());
    if (file_type == "bin") {
        std::cerr << std::hex;
        for (char ch : file) {
            std::cerr << (static_cast<int>(ch) & 0xFF) << ' ';
        }
        std::cerr << '\n' << std::dec;

        InputStream stream(file);
        x86::Decoder decoder(&stream);
        while (stream.has_more()) {
            auto inst = decoder.next_inst();
            inst.dump();
        }
    } else {
        throw std::runtime_error("Unknown file type " + file_type);
    }
}
