#pragma once

namespace bamf {

template <typename V>
class Edge {
    V *const m_src;
    V *const m_dst;

public:
    constexpr Edge(V *src, V *dst) : m_src(src), m_dst(dst) {}

    V *src() const { return m_src; }
    V *dst() const { return m_dst; }
};

} // namespace bamf
