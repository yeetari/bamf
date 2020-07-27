#pragma once

namespace bamf {

struct Statement {
    virtual void dump() const = 0;
};

} // namespace bamf
