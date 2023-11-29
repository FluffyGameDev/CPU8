#pragma once

#include <istream>
#include <unordered_map>
#include "../core/types.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;

    namespace Parser
    {
        class OpParser;
    }

    class RegisterBasedCompiler
    {
    public:
        RegisterBasedCompiler();

        bool CompileProgram(std::istream& input, MemoryUnit& rom, u8& programSize) const;

    private:
        std::unordered_map<u32, const Parser::OpParser*> m_Parsers;
    };
}