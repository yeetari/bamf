#pragma once

namespace bamf {

struct Statement {
    virtual ~Statement() = default;
    virtual void dump() const = 0;

    template <typename T>
    T *as();

    template <typename T>
    bool is();
};

template <typename T>
T *Statement::as() {
    return dynamic_cast<T *>(this);
}

template <typename T>
bool Statement::is() {
    return as<T>() != nullptr;
}

} // namespace bamf
