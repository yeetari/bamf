#include <bamf/ir/Program.hh>

#include <algorithm>
#include <cassert>

namespace bamf {

GlobalVariable *Program::add_global() {
    return m_globals.emplace_back(new GlobalVariable).get();
}

void Program::remove_global(GlobalVariable *global) {
    assert(global->users().empty());
    auto it = std::find_if(m_globals.begin(), m_globals.end(), [global](auto &ptr) {
        return ptr.get() == global;
    });
    if (it != m_globals.end()) {
        m_globals.erase(it);
    }
}

} // namespace bamf
