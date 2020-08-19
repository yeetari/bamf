#pragma once

#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

namespace bamf {

class Logger;

class Statistic {
    const Logger &m_logger;
    const char *m_str;
    mutable int m_value{0};

public:
    BAMF_MAKE_NON_COPYABLE(Statistic)
    BAMF_MAKE_NON_MOVABLE(Statistic)

    constexpr Statistic(const Logger &logger, const char *str) : m_logger(logger), m_str(str) {}
    ~Statistic();

    const Statistic &operator++() const;
};

} // namespace bamf
