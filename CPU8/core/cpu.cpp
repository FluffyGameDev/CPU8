#include "cpu.h"

#include "memoryunit.h"

namespace FluffyGamevev::CPU8
{
    void CPU::RunNextInstruction(const MemoryUnit& rom, MemoryUnit& ram)
    {
        u8 opCode{ rom.Buffer[InstructionPointer] };
        ++InstructionPointer;
        Instructions[opCode](*this, rom, ram);
    }
}