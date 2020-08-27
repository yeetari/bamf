#include <bamf/x86/Decoder.hh>

#include <bamf/support/Stream.hh>
#include <bamf/x86/MachineInst.hh>
#include <bamf/x86/Register.hh>

#include <cassert>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace bamf::x86 {

// clang-format off
#define BUILD(op, range, block) \
    do { \
        for (std::uint16_t i = op; i < (op) + (range); i++ ) { \
            auto &inst = m_table[i]; \
            inst.present = true; \
            inst.base_op = op; \
            do block \
            while (false); \
        } \
    } while (false)
// clang-format on

// clang-format off
#define BUILD_0F(op, range, block) \
    do { \
        for (std::uint16_t i = op; i < (op) + (range); i++ ) { \
            auto &inst = m_table_0f[i]; \
            inst.present = true; \
            inst.base_op = op; \
            do block \
            while (false); \
        } \
    } while (false)
// clang-format on

// clang-format off
#define BUILD_SLASH(op, range, slash, block) \
    do { \
        for (std::uint16_t i = op; i < (op) + (range); i++ ) { \
            auto &top_level = m_table[i]; \
            top_level.present = true; \
            top_level.has_slash = true; \
            if (top_level.slashes == nullptr) { \
                top_level.slashes = new InstructionInfo[8]; \
            } \
            auto &inst = top_level.slashes[slash]; \
            inst.present = true; \
            do block \
            while (false); \
        } \
    } while (false)
// clang-format on

Decoder::Decoder(Stream *stream) : m_stream(stream) {
    BUILD(0x31, 1, {
        inst.opcode = Opcode::Xor;
        inst.mod_rm = true;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmRm};
        inst.operands[1] = {OperandInfoType::ModRmGpr};
    });
    BUILD(0x50, 8, {
        inst.opcode = Opcode::Push;
        inst.default_operand_width = 64;
        inst.operands[0] = {OperandInfoType::OpcodeGpr};
    });
    BUILD(0x58, 8, {
        inst.opcode = Opcode::Pop;
        inst.default_operand_width = 64;
        inst.operands[0] = {OperandInfoType::OpcodeGpr};
    });
    BUILD(0x7D, 1, {
        inst.opcode = Opcode::Jge;
        inst.default_address_width = 8;
        inst.operands[0] = {OperandInfoType::Rel};
    });
    BUILD(0x89, 1, {
        inst.opcode = Opcode::Mov;
        inst.mod_rm = true;
        inst.default_address_width = 64;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmRm};
        inst.operands[1] = {OperandInfoType::ModRmGpr};
    });
    BUILD(0x8B, 1, {
        inst.opcode = Opcode::Mov;
        inst.mod_rm = true;
        inst.default_address_width = 64;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmGpr};
        inst.operands[1] = {OperandInfoType::ModRmRm};
    });
    BUILD(0xB8, 8, {
        inst.opcode = Opcode::Mov;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::OpcodeGpr};
        inst.operands[1] = {OperandInfoType::Imm};
    });
    BUILD(0xC3, 1, { inst.opcode = Opcode::Ret; });
    BUILD(0xC7, 1, {
        inst.opcode = Opcode::Mov;
        inst.mod_rm = true;
        inst.default_address_width = 64;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmRm};
        inst.operands[1] = {OperandInfoType::Imm};
    });
    BUILD(0xE8, 1, {
        inst.opcode = Opcode::Call;
        inst.default_address_width = 32;
        inst.operands[0] = {OperandInfoType::Rel};
    });
    BUILD(0xE9, 1, {
        inst.opcode = Opcode::Jmp;
        inst.default_address_width = 32;
        inst.operands[0] = {OperandInfoType::Rel};
    });
    BUILD(0xEB, 1, {
        inst.opcode = Opcode::Jmp;
        inst.default_address_width = 8;
        inst.operands[0] = {OperandInfoType::Rel};
    });
    BUILD_0F(0x8E, 1, {
        inst.opcode = Opcode::Jle;
        inst.default_address_width = 32;
        inst.operands[0] = {OperandInfoType::Rel};
    });
    BUILD_0F(0x8F, 1, {
        inst.opcode = Opcode::Jg;
        inst.default_address_width = 32;
        inst.operands[0] = {OperandInfoType::Rel};
    });
    BUILD_SLASH(0x83, 1, 7, {
        inst.opcode = Opcode::Cmp;
        inst.mod_rm = true;
        inst.default_address_width = 64;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmRm};
        inst.operands[1] = {OperandInfoType::Imm};
        inst.operands[1].imm_width = 8;
    });
    BUILD_SLASH(0xD1, 1, 4, {
        inst.opcode = Opcode::Shl;
        inst.mod_rm = true;
        inst.default_address_width = 32;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmRm};
        inst.operands[1] = {OperandInfoType::Constant};
        inst.operands[1].constant = 1;
    });
    BUILD_SLASH(0xFF, 1, 0, {
        inst.opcode = Opcode::Inc;
        inst.mod_rm = true;
        inst.default_operand_width = 32;
        inst.operands[0] = {OperandInfoType::ModRmRm};
    });
}

MachineInst Decoder::next_inst() {
    MachineInst inst{};
    inst.offset = m_stream->bytes_read();
    assert(m_stream->has_more());

    auto byte = m_stream->read<std::uint8_t>();
    inst.bytes[inst.length++] = byte;

    bool has_prefix = true;
    bool has_rex = false;
    struct {
        std::uint8_t w;
        std::uint8_t r;
        std::uint8_t x;
        std::uint8_t b;
    } rex{};

    switch (byte) {
    case 0x66:
        // Operand size override
        inst.operand_width = 16;
        break;
    case 0x67:
        // Address size override
        inst.address_width = 32;
        break;
    default:
        if ((byte & 0xF0U) >> 4U == 0b100U) {
            has_rex = true;
            rex.w = (byte >> 3U) & 0b1U;
            rex.r = (byte >> 2U) & 0b1U;
            rex.x = (byte >> 1U) & 0b1U;
            rex.b = (byte >> 0U) & 0b1U;
            if (rex.w == 1) {
                inst.operand_width = 64;
            }
            break;
        }

        has_prefix = false;
        break;
    }

    auto op = has_prefix ? m_stream->read<std::uint8_t>() : byte;
    if (has_prefix) {
        inst.bytes[inst.length++] = op;
    }

    bool is_0f = op == 0x0F;
    if (is_0f) {
        op = m_stream->read<std::uint8_t>();
        inst.bytes[inst.length++] = op;
    }

    auto *info = !is_0f ? &m_table[op] : &m_table_0f[op];
    if (!info->present) {
        std::stringstream ss;
        ss << "Unknown opcode: " << std::hex;
        ss << (static_cast<unsigned int>(op) & 0xFFU);
        throw std::runtime_error(ss.str());
    }

    struct {
        std::uint8_t mod;
        std::uint8_t reg;
        std::uint8_t rm;
    } mod_rm{};
    if (info->mod_rm || info->has_slash) {
        auto mod_rm_byte = m_stream->read<std::uint8_t>();
        inst.bytes[inst.length++] = mod_rm_byte;
        mod_rm.mod = (mod_rm_byte >> 6U) & 0b11U;
        mod_rm.reg = (mod_rm_byte >> 3U) & 0b111U;
        mod_rm.rm = mod_rm_byte & 0b111U;
    }

    if (info->has_slash) {
        info = &info->slashes[mod_rm.reg];
        if (!info->present) {
            std::stringstream ss;
            ss << "Unknown opcode: " << std::hex;
            ss << (static_cast<unsigned int>(op) & 0xFFU);
            ss << " /" << (static_cast<unsigned int>(mod_rm.reg) & 0xFFU);
            throw std::runtime_error(ss.str());
        }
    }

    inst.opcode = info->opcode;
    inst.address_width = inst.address_width == 0 ? info->default_address_width : inst.address_width;
    inst.operand_width = inst.operand_width == 0 ? info->default_operand_width : inst.operand_width;

    for (int i = 0; i < 4; i++) {
        const auto &operand_info = info->operands[i];
        auto &operand = inst.operands[i];
        if (operand_info.type == OperandInfoType::None) {
            break;
        }

        switch (operand_info.type) {
        case OperandInfoType::Constant:
        case OperandInfoType::Imm: {
            std::uint8_t width = operand_info.type == OperandInfoType::Imm ? inst.operand_width : inst.address_width;
            if (operand_info.type == OperandInfoType::Constant) {
                width = 0;
            } else if (operand_info.type == OperandInfoType::Imm) {
                width = operand_info.imm_width != 0 ? operand_info.imm_width : width;
            }

            operand.type = OperandType::Imm;
            switch (width) {
            case 8:
                operand.imm = m_stream->read<std::uint8_t>();
                break;
            case 16:
                operand.imm = m_stream->read<std::uint16_t>();
                break;
            case 32:
                operand.imm = m_stream->read<std::uint32_t>();
                break;
            case 64:
                operand.imm = m_stream->read<std::uint64_t>();
                break;
            default:
                operand.imm = operand_info.constant;
                break;
            }

            for (int j = 0; j < width / 8; j++) {
                inst.bytes[inst.length++] = operand.imm_bytes[j];
            }
            break;
        }
        case OperandInfoType::ModRmGpr:
            operand.type = OperandType::Reg;
            operand.reg = static_cast<Register>(mod_rm.reg);
            break;
        case OperandInfoType::ModRmRm: {
            struct {
                std::uint8_t scale;
                std::uint8_t index;
                std::uint8_t base;
            } sib{};
            bool has_sib = mod_rm.rm == 0b100;
            if (has_sib) {
                auto sib_byte = m_stream->read<std::uint8_t>();
                inst.bytes[inst.length++] = sib_byte;
                sib.scale = (sib_byte >> 6U) & 0b11U;
                sib.index = (sib_byte >> 3U) & 0b111U;
                sib.base = sib_byte & 0b111U;
            }

            auto reg = static_cast<Register>(mod_rm.rm);
            if (mod_rm.mod == 0b11) {
                // reg
                operand.type = OperandType::Reg;
                operand.reg = reg;
                break;
            }

            // [reg]
            operand.type = OperandType::MemBaseIndexScale;
            operand.base = reg;
            if (has_sib) {
                operand.base = static_cast<Register>(sib.base);
                operand.index = static_cast<Register>(sib.index);
                operand.scale = 1U << sib.scale;
            }

            if (mod_rm.mod == 0b01) {
                // [reg] + disp8
                auto disp_byte = m_stream->read<std::uint8_t>();
                inst.bytes[inst.length++] = disp_byte;
                operand.type = OperandType::MemBaseDisp;
                operand.disp = disp_byte;
                break;
            }
            throw std::runtime_error("Unsupported ModRM addressing mode");
        }
        case OperandInfoType::OpcodeGpr:
            operand.type = OperandType::Reg;
            operand.reg = static_cast<Register>(op - info->base_op);
            break;
        case OperandInfoType::Rel: {
            operand.type = OperandType::Imm;
            switch (inst.address_width) {
            case 8:
                operand.imm = static_cast<std::int8_t>(m_stream->read<std::uint8_t>());
                break;
            case 16:
                operand.imm = static_cast<std::int16_t>(m_stream->read<std::uint16_t>());
                break;
            case 32:
                operand.imm = static_cast<std::int32_t>(m_stream->read<std::uint32_t>());
                break;
            case 64:
                operand.imm = static_cast<std::int64_t>(m_stream->read<std::uint64_t>());
                break;
            }
            for (int j = 0; j < inst.address_width / 8; j++) {
                inst.bytes[inst.length++] = operand.imm_bytes[j];
            }
            operand.imm += inst.length + inst.offset;
            break;
        }
        default:
            throw std::runtime_error("Unsupported operand decode method");
        }
    }
    return inst;
}

bool Decoder::has_next() const {
    return m_stream->has_more();
}

} // namespace bamf::x86
