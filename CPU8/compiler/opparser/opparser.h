#pragma once

#include <string_view>
#include "../../core/types.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;

    namespace Parser
    {
        class OpParser
        {
        public:
            virtual ~OpParser() = default;
            virtual bool Parse(std::string_view& line, MemoryUnit& rom, u8& programSize) const = 0;
        };
    }
}