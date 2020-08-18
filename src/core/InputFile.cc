#include <bamf/core/InputFile.hh>

#include <fstream>
#include <stdexcept>
#include <string>

namespace bamf {

InputFile::InputFile(const char *path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file " + std::string(path) + "!");
    }

    m_size = file.tellg();
    m_data.resize(m_size);
    file.seekg(0);
    file.read(m_data.data(), m_size);
}

} // namespace bamf
