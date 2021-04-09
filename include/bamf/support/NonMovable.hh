#pragma once

#define BAMF_MAKE_NON_MOVABLE(clazz)                                                                                   \
    clazz(clazz &&) noexcept = delete;                                                                                 \
    clazz &operator=(clazz &&) = delete;
