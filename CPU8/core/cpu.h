#pragma once

#include <vector>
#include "types.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;

    struct CPU
    {
        using InstructionCallback = void(*)(CPU&, const MemoryUnit&, MemoryUnit&);

        void RunNextInstruction(const MemoryUnit& rom, MemoryUnit& ram);

        InstructionCallback Instructions[256];
        std::vector<u8> Registers;
        u8 InstructionPointer;
        u8 Flags;
    };
}