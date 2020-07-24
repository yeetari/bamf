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
    case Register::Rdx:
        switch (bit_width) {
        case 16:
            return "dx";
        case 32:
            return "edx";
        case 64:
            return "rdx";
        default:
            return "invalid";
        }
    case Register::Rbx:
        switch (bit_width) {
        case 16:
            return "bx";
        case 32:
            return "ebx";
        case 64:
            return "rbx";
        default:
            return "invalid";
        }
    case Register::Rsp:
        switch (bit_width) {
        case 16:
            return "sp";
        case 32:
            return "esx";
        case 64:
            return "rsp";
        default:
            return "invalid";
        }
    case Register::Rbp:
        switch (bit_width) {
        case 16:
            return "bp";
        case 32:
            return "ebp";
        case 64:
            return "rbp";
        default:
            return "invalid";
        }
    case Register::Rsi:
        switch (bit_width) {
        case 16:
            return "si";
        case 32:
            return "esi";
        case 64:
            return "rsi";
        default:
            return "invalid";
        }
    case Register::Rdi:
        switch (bit_width) {
        case 16:
            return "di";
        case 32:
            return "edi";
        case 64:
            return "rdi";
        default:
            return "invalid";
        }
    case Register::R8:
        switch (bit_width) {
        case 16:
            return "r8w";
        case 32:
            return "r8d";
        case 64:
            return "r8";
        default:
            return "invalid";
        }
    case Register::R9:
        switch (bit_width) {
        case 16:
            return "r9w";
        case 32:
            return "r9d";
        case 64:
            return "r9";
        default:
            return "invalid";
        }
    case Register::R10:
        switch (bit_width) {
        case 16:
            return "r10w";
        case 32:
            return "r10d";
        case 64:
            return "r10";
        default:
            return "invalid";
        }
    case Register::R11:
        switch (bit_width) {
        case 16:
            return "r11w";
        case 32:
            return "r11d";
        case 64:
            return "r11";
        default:
            return "invalid";
        }
    case Register::R12:
        switch (bit_width) {
        case 16:
            return "r12w";
        case 32:
            return "r12d";
        case 64:
            return "r12";
        default:
            return "invalid";
        }
    case Register::R13:
        switch (bit_width) {
        case 16:
            return "r13w";
        case 32:
            return "r13d";
        case 64:
            return "r13";
        default:
            return "invalid";
        }
    case Register::R14:
        switch (bit_width) {
        case 16:
            return "r14w";
        case 32:
            return "r14d";
        case 64:
            return "r14";
        default:
            return "invalid";
        }
    case Register::R15:
        switch (bit_width) {
        case 16:
            return "r15w";
        case 32:
            return "r15d";
        case 64:
            return "r15";
        default:
            return "invalid";
        }
    }
}

} // namespace bamf::x86
