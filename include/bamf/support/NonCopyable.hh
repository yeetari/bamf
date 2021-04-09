#pragma once

#define BAMF_MAKE_NON_COPYABLE(clazz)                                                                                  \
    clazz(const clazz &) = delete;                                                                                     \
    clazz &operator=(const clazz &) = delete;
