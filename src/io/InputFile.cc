#include <bamf/io/InputFile.hh>

#include <fstream>
#include <stdexcept>

namespace bamf {

InputFile::InputFile(const char *path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file " + std::string(path) + "!");
    }

    const auto size = file.tellg();
    m_data.resize(size);
    file.seekg(0);
    file.read(m_data.data(), size);
}

} // namespace bamf
