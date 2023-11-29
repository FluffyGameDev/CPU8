#include <iostream>
#include <sstream>

#include "core/cpu.h"
#include "core/characterdisplay.h"
#include "core/memoryunit.h"
#include "core/registerbasedcpu.h"
#include "compiler/codegenerator.h"

const std::string k_Program
{
    "CONST gridSide 8\n"
    "CONST maxLoop 4\n"
    "CONST videoStart 191\n"
    "CONST charOn 35\n"
    "DAT dx 1\n"
    "DAT dy 0\n"
    "DAT x 0\n"
    "DAT y 0\n"
    "DAT i 0\n"
    "DAT j 0\n"
    "DAT k 0\n"
    "MOV A i\n"
    "MOV [A] maxLoop\n"
    "loop_i_start:\n"
    "    MOV A maxLoop\n"
    "    LOAD B i\n"
    "    SUB A B\n"
    "    MOV B x\n"
    "    MOV [B] A\n"
    "    MOV B y\n"
    "    MOV [B] A\n"
    "    MOV A j\n"
    "    MOV [A] 0\n"
    "    loop_j_start:\n"
    "        MOV A k\n"
    "        MOV [A] 0\n"
    "        loop_k_start:\n"
    "            MOV A videoStart\n"
    "            LOAD B y\n"
    "            MUL B gridSide\n"
    "            ADD A B\n"
    "            LOAD B x\n"
    "            ADD A B\n"
    "            MOV [A] charOn\n"
    "            LOAD A x\n"
    "            LOAD B dx\n"
    "            ADD A B\n"
    "            MOV B x\n"
    "            MOV [B] A\n"
    "            LOAD A y\n"
    "            LOAD B dy\n"
    "            ADD A B\n"
    "            MOV B y\n"
    "            MOV [B] A\n"
    "            LOAD A k\n"
    "            MOV B k\n"
    "            INC A\n"
    "            MOV [B] A\n"
    "            LOAD B i\n"
    "            MUL B 2\n"
    "            DEC B\n"
    "            LOAD A k\n"
    "            CMP A B\n"
    "            JL loop_k_start\n"
    "    LOAD A dx\n"
    "    LOAD B dy\n"
    "    MOV C A\n"
    "    MOV A B\n"
    "    MOV B C\n"
    "    NOT A\n"
    "    INC A\n"
    "    MOV C dx\n"
    "    MOV [C] A\n"
    "    MOV C dy\n"
    "    MOV [C] B\n"
    "    LOAD A j\n"
    "    MOV B j\n"
    "    INC A\n"
    "    MOV [B] A\n"
    "    LOAD A j\n"
    "    CMP A 4\n"
    "    JL loop_j_start\n"
    "LOAD A i\n"
    "MOV B i\n"
    "DEC A\n"
    "MOV [B] A\n"
    "LOAD A i\n"
    "CMP A 0\n"
    "JG loop_i_start\n"
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
