#pragma once

#include <vector>

#include "lexer.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;
}

namespace FluffyGamevev::CPU8::Compiler
{
    class CodeGenerator
    {
    public:
        CodeGenerator();

        bool CompileProgram(const std::vector<LexerToken>& tokens, MemoryUnit& rom, u8 programSize) const;
    };
}