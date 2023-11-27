#include <chrono>
#include <iostream>
#include <thread>
#include <Windows.h>

#include "core/types.h"
#include "core/cpu.h"
#include "core/characterdisplay.h"
#include "core/memoryunit.h"

int main()
{
    using namespace FluffyGamevev::CPU8;
    using namespace std::chrono_literals;

    CPU cpu{};
    MemoryUnit ram{};
    MemoryUnit rom{};
    CharacterDisplay display{};
    u8 programSize{};

    //TODO: compile program
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

    /*
        MOV A 'A'
        MOV B VIDEO_START
        MOV C VIDEO_START
        ADD C 26

    loopStart:
        MOV [B] A
        INC A
        INC B

        CMP B C
        JG loopStart
    */

    while (cpu.InstructionPointer < programSize)
    {
        cpu.RunNextInstruction(rom, ram);
        display.RenderDisplay(ram);
    }

    return 0;
}
