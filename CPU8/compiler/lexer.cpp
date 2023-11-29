#include "lexer.h"

namespace FluffyGamevev::CPU8::Compiler
{
    namespace Internal
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
    }

    Lexer::Lexer()
    {
    }

    bool Lexer::BuildTokenSequence(const std::string& translationUnit, std::vector<LexerToken>& outputTokens) const
    {
        return false;
    }
}