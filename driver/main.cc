#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

void print_usage(const char *prog_name) {
    std::cerr << "Usage: " << prog_name << " <file>\n";
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    std::ifstream file(argv[1], std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file " + std::string(argv[1]) + "!");
    }

    const auto size = file.tellg();
    std::vector<char> data(size);
    file.seekg(0);
    file.read(data.data(), size);

    std::cerr << "Parsing: " << std::hex;
    for (char ch : data) {
        std::cerr << (static_cast<int>(ch) & 0xFF) << ' ';
    }
    std::cerr << '\n';
}
