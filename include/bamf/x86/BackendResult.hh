#pragma once

#include <bamf/pass/PassResult.hh>
#include <bamf/x86/MachineInst.hh>

#include <vector>

namespace bamf::x86 {

class BackendResult : public PassResult, public std::vector<MachineInst> {};

} // namespace bamf::x86
