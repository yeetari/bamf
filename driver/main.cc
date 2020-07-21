#include <bamf/io/InputFile.hh>

#include <iostream>

using namespace bamf;

namespace {

void print_usage(const char *prog_name) {
    std::cerr << "Usage: " << prog_name << " <file>\n";
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::cerr << "Parsing: " << std::hex;
    InputFile input(argv[1]);
    for (char ch : input) {
        std::cerr << (static_cast<int>(ch) & 0xFF) << ' ';
    }
    std::cerr << '\n';
}
