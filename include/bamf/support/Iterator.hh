#pragma once

// TODO(): reverse_iterator and const_reverse_iterator
#define BAMF_MAKE_ITERABLE(list) \
    using iterator = typename decltype(list)::iterator; \
    using const_iterator = typename decltype(list)::const_iterator; \
    iterator begin() { return list.begin(); } \
    iterator end() { return list.end(); } \
    const_iterator begin() const { return list.begin(); } \
    const_iterator end() const { return list.end(); }
