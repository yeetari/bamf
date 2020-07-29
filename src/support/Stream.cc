#include <bamf/support/Stream.hh>

#include <cstdint>

namespace bamf {

template <>
std::uint8_t Stream::read() {
    return static_cast<std::uint8_t>(m_data[m_bytes_read++]);
}

template <>
std::uint16_t Stream::read() {
    return read<std::uint8_t>() | (static_cast<std::uint16_t>(read<std::uint8_t>()) << 8U);
}

template <>
std::uint32_t Stream::read() {
    return read<std::uint16_t>() | (static_cast<std::uint32_t>(read<std::uint16_t>()) << 16U);
}

template <>
std::uint64_t Stream::read() {
    return read<std::uint32_t>() | (static_cast<std::uint64_t>(read<std::uint32_t>()) << 32U);
}

} // namespace bamf
