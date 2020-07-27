#pragma once

namespace bamf {

struct Statement {
    virtual ~Statement() = default;
    virtual void dump() const = 0;
};

} // namespace bamf
