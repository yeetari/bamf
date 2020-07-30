#pragma once

namespace bamf {

struct Expression {
    virtual ~Expression() = default;
    virtual void dump() const = 0;

    template <typename T>
    T *as();

    template <typename T>
    bool is();
};

template <typename T>
T *Expression::as() {
    return dynamic_cast<T *>(this);
}

template <typename T>
bool Expression::is() {
    return as<T>() != nullptr;
}

} // namespace bamf
