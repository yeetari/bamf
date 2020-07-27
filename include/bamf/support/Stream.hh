#pragma once

#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <cstdint>

namespace bamf {

class Stream {
    const char *const m_data;
    const std::size_t m_length;
    std::size_t m_bytes_read{0};

public:
    BAMF_MAKE_NON_COPYABLE(Stream)
    BAMF_MAKE_NON_MOVABLE(Stream)

    constexpr Stream(const char *data, std::size_t length) : m_data(data), m_length(length) {}
    ~Stream() = default;

    template <typename T>
    T read();

    bool has_more() const { return m_bytes_read < m_length; }
    std::size_t bytes_read() const { return m_bytes_read; }
};

} // namespace bamf
