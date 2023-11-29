#pragma once

#include "opparser.h"

namespace FluffyGamevev::CPU8::Parser
{
    class NoArgParser : public OpParser
    {
    public:
        NoArgParser(u8 opCode);

        bool Parse(std::string_view& line, MemoryUnit& rom, u8& programSize) const override;

    private:
        u8 m_OpCode;
    };
}