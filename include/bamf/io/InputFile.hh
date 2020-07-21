#pragma once

#include <bamf/core/Iterator.hh>
#include <bamf/core/NonCopyable.hh>
#include <bamf/core/NonMovable.hh>

#include <vector>

namespace bamf {

class InputFile {
    std::vector<char> m_data;

public:
    BAMF_MAKE_ITERABLE(m_data)
    BAMF_MAKE_NON_COPYABLE(InputFile)
    BAMF_MAKE_NON_MOVABLE(InputFile)

    InputFile(const char *path);
};

} // namespace bamf
