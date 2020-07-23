#pragma once

namespace bamf {

class Executable;
class InputFile;

void load_elf_exec(const InputFile &file, Executable *executable);

} // namespace bamf
