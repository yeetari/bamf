#pragma once

#include <bamf/core/Iterator.hh>
#include <bamf/core/NonCopyable.hh>
#include <bamf/core/NonMovable.hh>

#include <cstdint>
#include <vector>

namespace bamf {

class InputFile {
    std::vector<char> m_data;
    std::size_t m_size;

public:
    BAMF_MAKE_ITERABLE(m_data)
    BAMF_MAKE_NON_COPYABLE(InputFile)
    BAMF_MAKE_NON_MOVABLE(InputFile)

    InputFile(const char *path);

    template <typename T>
    const T *get(std::ptrdiff_t offset) const {
        return reinterpret_cast<const T *>(m_data.data() + offset);
    }

    std::size_t size() const { return m_size; }
};

} // namespace bamf
