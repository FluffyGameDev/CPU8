#pragma once

#include <string_view>

#include "../core//types.h"

namespace FluffyGamevev::CPU8::ParserUtils
{
    void SkipWhitespaces(std::string_view& lineView);
    bool FindNextToken(std::string_view& lineView, std::string_view& foundToken);
    u32 ConvertTokenToU32(const std::string_view& token);
}