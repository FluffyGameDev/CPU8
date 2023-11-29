#pragma once

#include <unordered_map>
#include <vector>

#include "lexer.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;
}

namespace FluffyGamevev::CPU8::Compiler
{
    using TokensIterator = std::vector<LexerToken>::const_iterator;
    using CodeGeneratorHandler = bool (*)(std::unordered_map<std::string, u8>&, TokensIterator&, TokensIterator, MemoryUnit&, u8&, u8&);

    class CodeGenerator
    {
    public:
        CodeGenerator();

        bool CompileProgram(const std::vector<LexerToken>& tokens, MemoryUnit& rom, u8& programSize) const;

    private:
        std::vector<CodeGeneratorHandler> m_CodeGenerators;
    };
}