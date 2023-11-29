#include "parserutils.h"

#include <string>

namespace FluffyGamevev::CPU8::ParserUtils
{
    void SkipWhitespaces(std::string_view& lineView)
    {
        while (!lineView.empty() && std::isspace(lineView.front()))
        {
            lineView.remove_prefix(1);
        }
    }

    bool FindNextToken(std::string_view& lineView, std::string_view& foundToken)
    {
        SkipWhitespaces(lineView);

        u32 tokenLength{};
        while (tokenLength < lineView.size() && std::isgraph(lineView[tokenLength]))
        {
            ++tokenLength;
        }

        foundToken = lineView;
        lineView.remove_prefix(tokenLength);
        foundToken.remove_suffix(foundToken.size() - tokenLength);

        return tokenLength > 0;
    }

    u32 ConvertTokenToU32(const std::string_view& token)
    {
        u32 result{};
        for (char c : token)
        {
            result <<= 8;
            result |= (u32)(c);
        }
        return result;
    }
}