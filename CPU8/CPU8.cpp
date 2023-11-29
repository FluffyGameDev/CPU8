#include <iostream>
#include <sstream>

#include "core/cpu.h"
#include "core/characterdisplay.h"
#include "core/memoryunit.h"
#include "core/registerbasedcpu.h"
#include "compiler/codegenerator.h"

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
const std::string k_Program
{
    "CONST CHAR_A 65\n"
    "CONST VIDEO_START 191\n"
    "MOV A CHAR_A\n" //0xdc 0x41
    "MOV B VIDEO_START\n" //0xdd 0xbf
    "MOV C VIDEO_START\n" //0xde 0xbf
    "ADD C 26\n" //0xb2 0x1a
    "loopStart:\n"
    "MOV [B] A\n" //0x14
    "INC A\n" //0xd4
    "INC B\n" //0xd5
    "CMP B C\n" //0xa6
    "JL loopStart\n" //0xfd 0x08
};

int main()
{
    using namespace FluffyGamevev::CPU8;

    CPU cpu{};
    MemoryUnit ram{};
    MemoryUnit rom{};
    CharacterDisplay display{};
    u8 programSize{};

    Compiler::Lexer lexer{};
    Compiler::CodeGenerator codeGenerator{};
    std::vector<Compiler::LexerToken> tokens{};

    InitCPU(cpu);
    if (lexer.BuildTokenSequence(k_Program, tokens) &&
        codeGenerator.CompileProgram(tokens, rom, programSize))
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
