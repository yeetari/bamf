#pragma once

namespace bamf {

struct Expression {
    virtual void dump() const = 0;
};

} // namespace bamf
