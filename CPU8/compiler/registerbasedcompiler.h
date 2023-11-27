#pragma once

#include <istream>
#include "../core/types.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;

    namespace RegisterBasedCompiler
    {
        bool CompileProgram(std::istream& input, MemoryUnit& rom, u8& programSize);
    }
}