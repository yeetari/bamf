#pragma once

#include <bamf/core/NonCopyable.hh>
#include <bamf/core/NonMovable.hh>

#include <vector>

namespace bamf {

class InputFile;

class InputStream {
    const InputFile &m_file;
    std::vector<char>::const_iterator m_it;
    std::size_t m_bytes_read{0};

public:
    BAMF_MAKE_NON_COPYABLE(InputStream)
    BAMF_MAKE_NON_MOVABLE(InputStream)

    InputStream(const InputFile &file);

    template <typename T>
    T read();

    bool has_more() const;
    std::size_t bytes_read() const { return m_bytes_read; }
};

} // namespace bamf
