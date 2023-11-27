#include <iostream>
#include <sstream>

#include "core/cpu.h"
#include "core/characterdisplay.h"
#include "core/memoryunit.h"
#include "compiler/registerbasedcompiler.h"

/*
    char dstAddr = VIDEO_START;
    char c = 'A';
    while (dstAddr < VIDEO_START + 26)
    {
        *dstAddr = c;
        ++dstAddr;
        ++c;
    }
*/
const char* k_Program
{
    "MOV A 'A'\n"
    "MOV B VIDEO_START\n"
    "MOV C VIDEO_START\n"
    "ADD C 26\n"
    "loopStart:\n"
    "MOV[B] A\n"
    "INC A\n"
    "INC B\n"
    "CMP B C\n"
    "JG loopStart\n"
};

int main()
{
    using namespace FluffyGamevev::CPU8;

    CPU cpu{};
    MemoryUnit ram{};
    MemoryUnit rom{};
    CharacterDisplay display{};
    u8 programSize{};

    std::istringstream ss{ k_Program };
    if (RegisterBasedCompiler::CompileProgram(ss, rom, programSize))
    {
        while (cpu.InstructionPointer < programSize)
        {
            cpu.RunNextInstruction(rom, ram);
            display.RenderDisplay(ram);
        }
    }
    else
    {
        std::cout << "Failed to compile Program.\n";
        //TODO: print error details
    }

    return 0;
}
