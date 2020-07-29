#include <bamf/io/ElfLoader.hh>

#include <bamf/core/Executable.hh>
#include <bamf/io/InputFile.hh>

#include <array>
#include <cstdint>
#include <string_view>

namespace bamf {

namespace {

enum class Arch : std::uint8_t {
    Elf32 = 1,
    Elf64 = 2,
};

enum class Endian : std::uint8_t {
    Little = 1,
    Big = 2,
};

enum class Version : std::uint8_t {
    Current = 1,
};

enum class Abi : std::uint8_t {
    SystemV = 0,
};

enum class Type : std::uint16_t {
    None = 0,
    Relocatable = 1,
    Executable = 2,
    Dynamic = 3,
    Core = 4,
};

enum class Machine : std::uint16_t {
    None = 0,
    Amd64 = 62,
};

struct Header {
    // ELF identification
    std::array<std::uint8_t, 4> magic;
    Arch arch;
    Endian endian;
    Version version;
    Abi abi;
    std::uint64_t pad_1;

    Type type;
    Machine machine;
    std::uint32_t obj_ver;
    std::uint64_t entry;
    std::uint64_t ph_off;
    std::uint64_t sh_off;
    std::uint32_t flags;
    std::uint16_t header_size;
    std::uint16_t ph_size;
    std::uint16_t ph_count;
    std::uint16_t sh_size;
    std::uint16_t sh_count;
    std::uint16_t sh_str_tbl_off;
};

enum class SectionHeaderType : std::uint32_t {
    StrTbl = 3,
};

struct SectionHeader {
    std::uint32_t name;
    SectionHeaderType type;
    std::uint64_t flags;
    std::uint64_t virt_addr;
    std::uint64_t offset;
    std::uint64_t size;
    std::uint32_t link;
    std::uint32_t info;
    std::uint64_t align;
    std::uint64_t entry_size;
};

} // namespace

void load_elf_exec(const InputFile &file, Executable *executable) {
    const auto *header = file.get<Header>(0);
    executable->entry_point = header->entry;

    auto get_section_header = [&](int idx) -> const SectionHeader * {
        return file.get<SectionHeader>(header->sh_off + idx * header->sh_size);
    };

    // Find string table
    const SectionHeader *str_tbl = nullptr;
    for (int i = 0; i < header->sh_count; i++) {
        const auto *sh = get_section_header(i);
        if (sh->type == SectionHeaderType::StrTbl) {
            str_tbl = sh;
        }
    }

    for (int i = 0; i < header->sh_count; i++) {
        const auto *sh = get_section_header(i);
        std::string_view name(file.get<char>(str_tbl->offset + sh->name));
        if (name != ".text") {
            continue;
        }

        executable->code = file.get<std::uint8_t>(sh->offset);
        executable->code_size = sh->size;
    }
}

} // namespace bamf
