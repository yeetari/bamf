#pragma once

#include <bamf/support/Iterable.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <cstdint>
#include <vector>

namespace bamf {

template <typename T>
class Stack {
    std::vector<T *> m_impl;

public:
    BAMF_MAKE_ITERABLE(m_impl)
    BAMF_MAKE_NON_COPYABLE(Stack)
    BAMF_MAKE_NON_MOVABLE(Stack)

    Stack() = default;
    ~Stack() = default;

    constexpr void push(T *value);
    constexpr T *peek() const;
    constexpr T *pop();

    constexpr bool empty() const { return m_impl.empty(); }
    constexpr std::size_t size() const { return m_impl.size(); }
};

template <typename T>
constexpr void Stack<T>::push(T *value) {
    m_impl.push_back(value);
}

template <typename T>
constexpr T *Stack<T>::peek() const {
    return m_impl.back();
}

template <typename T>
constexpr T *Stack<T>::pop() {
    auto *ret = m_impl.back();
    m_impl.pop_back();
    return ret;
}

} // namespace bamf
