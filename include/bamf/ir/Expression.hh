#pragma once

namespace bamf {

struct Expression {
    virtual ~Expression() = default;
    virtual void dump() const = 0;
};

} // namespace bamf
