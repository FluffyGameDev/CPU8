#include "noargparser.h"

#include "../../core/memoryunit.h"

namespace FluffyGamevev::CPU8::Parser
{
    NoArgParser::NoArgParser(u8 opCode)
        : m_OpCode{ opCode }
    {
    }

    bool NoArgParser::Parse(std::string_view& line, MemoryUnit& rom, u8& programSize) const
    {
        rom.Buffer[programSize] = m_OpCode;
        ++programSize;
        return true;
    }
}