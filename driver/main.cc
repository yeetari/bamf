#include <bamf/io/InputFile.hh>
#include <bamf/io/InputStream.hh>
#include <bamf/x86/Decoder.hh>

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

    std::cerr << "Decoding: " << std::hex;
    InputFile input(argv[1]);
    for (char ch : input) {
        std::cerr << (static_cast<int>(ch) & 0xFF) << ' ';
    }
    std::cerr << '\n';

    InputStream stream(input);
    x86::Decoder decoder(&stream);
    while (stream.has_more()) {
        std::cout << std::hex << stream.bytes_read() << ": " << std::dec;
        auto inst = decoder.next_inst();
        inst.dump();
    }
}
