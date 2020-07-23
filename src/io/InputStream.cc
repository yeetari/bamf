#include <bamf/io/InputStream.hh>

#include <bamf/io/InputFile.hh>

#include <cstdint>

namespace bamf {

InputStream::InputStream(const InputFile &file) : m_file(file), m_it(file.begin()) {}

template <>
std::uint8_t InputStream::read() {
    m_bytes_read++;
    return static_cast<std::uint8_t>(*m_it++);
}

template <>
std::uint16_t InputStream::read() {
    return read<std::uint8_t>() | (read<std::uint8_t>() << 8U);
}

template <>
std::uint32_t InputStream::read() {
    return read<std::uint16_t>() | (read<std::uint16_t>() << 16U);
}

bool InputStream::has_more() const {
    return m_it != m_file.end();
}

} // namespace bamf
