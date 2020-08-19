#include <bamf/pass/Statistic.hh>

#include <bamf/support/Logger.hh>

namespace bamf {

Statistic::~Statistic() {
    m_logger.trace(m_str, m_value);
}

const Statistic &Statistic::operator++() const {
    m_value++;
    return *this;
}

} // namespace bamf
