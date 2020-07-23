#include <bamf/x86/Register.hh>

namespace bamf::x86 {

const char *reg_to_str(Register reg, int bit_width) {
    switch (reg) {
    case Register::Rax:
        switch (bit_width) {
        case 16:
            return "ax";
        case 32:
            return "eax";
        case 64:
            return "rax";
        default:
            return "invalid";
        }
    case Register::Rcx:
        switch (bit_width) {
        case 16:
            return "cx";
        case 32:
            return "ecx";
        case 64:
            return "rcx";
        default:
            return "invalid";
        }
    }
}

} // namespace bamf::x86
