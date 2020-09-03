#include <bamf/core/Executable.hh>

#include <bamf/core/ElfLoader.hh>
#include <bamf/core/InputFile.hh>

namespace bamf {

Executable load_executable(const InputFile &file) {
    Executable executable{};
    executable.code = file.get<std::uint8_t>(0);
    executable.code_size = file.size();
    if (executable.code[0] == 0x7F && executable.code[1] == 0x45 && executable.code[2] == 0x4C &&
        executable.code[3] == 0x46) {
        load_elf_exec(file, &executable);
    }
    return executable;
}

} // namespace bamf
