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

        bool ReadNextLine(std::string_view& text, std::string_view& line)
        {
            u32 endLineIndex{ (u32)std::min(text.find_first_of('\n') + 1, text.size()) };
            line = std::string_view(text.data(), endLineIndex);
            text.remove_prefix(endLineIndex);
            return line.size() > 0;
        }

        bool ReadNextToken(std::string_view& line, std::string_view& token)
        {
            SkipWhitespaces(line);

            u32 tokenLength{};
            while (tokenLength < line.size() && std::isgraph(line[tokenLength]))
            {
                ++tokenLength;
            }

            token = line;
            line.remove_prefix(tokenLength);
            token.remove_suffix(token.size() - tokenLength);

            return tokenLength > 0;
        }

        using TokenPredicate = int (*)(int);
        bool DoesTokenMatchPredicate(const std::string_view& token, TokenPredicate predicate)
        {
            bool doesMatch{ true };
            for (char c : token)
            {
                if (!predicate(c))
                {
                    doesMatch = false;
                    break;
                }
            }
            return doesMatch;
        }

        int IsValidText(int c)
        {
            return (int)(std::isalnum(c) || c == '_');
        }
    }

    Lexer::Lexer()
    {
        m_ReservedSymbols["A"] = LexerTokenType::RegisterA;
        m_ReservedSymbols["B"] = LexerTokenType::RegisterB;
        m_ReservedSymbols["C"] = LexerTokenType::RegisterC;
        m_ReservedSymbols["SP"] = LexerTokenType::RegisterSP;
        m_ReservedSymbols["[A]"] = LexerTokenType::RegisterPointerA;
        m_ReservedSymbols["[B]"] = LexerTokenType::RegisterPointerB;
        m_ReservedSymbols["[C]"] = LexerTokenType::RegisterPointerC;
        m_ReservedSymbols["[SP]"] = LexerTokenType::RegisterPointerSP;
        m_ReservedSymbols["NOP"] = LexerTokenType::Operator_Nop;
        m_ReservedSymbols["CONST"] = LexerTokenType::Operator_Const;
        m_ReservedSymbols["DAT"] = LexerTokenType::Operator_Dat;
        m_ReservedSymbols["MOV"] = LexerTokenType::Operator_Mov;
        m_ReservedSymbols["LOAD"] = LexerTokenType::Operator_Load;
        m_ReservedSymbols["PUSH"] = LexerTokenType::Operator_Push;
        m_ReservedSymbols["POP"] = LexerTokenType::Operator_Pop;
        m_ReservedSymbols["CMP"] = LexerTokenType::Operator_Cmp;
        m_ReservedSymbols["INC"] = LexerTokenType::Operator_Inc;
        m_ReservedSymbols["DEC"] = LexerTokenType::Operator_Dec;
        m_ReservedSymbols["ADD"] = LexerTokenType::Operator_Add;
        m_ReservedSymbols["SUB"] = LexerTokenType::Operator_Sub;
        m_ReservedSymbols["MUL"] = LexerTokenType::Operator_Mul;
        m_ReservedSymbols["DIV"] = LexerTokenType::Operator_Div;
        m_ReservedSymbols["MOD"] = LexerTokenType::Operator_Mod;
        m_ReservedSymbols["AND"] = LexerTokenType::Operator_And;
        m_ReservedSymbols["OR"] = LexerTokenType::Operator_Or;
        m_ReservedSymbols["XOR"] = LexerTokenType::Operator_Xor;
        m_ReservedSymbols["NOT"] = LexerTokenType::Operator_Not;
        m_ReservedSymbols["JMP"] = LexerTokenType::Operator_Jmp;
        m_ReservedSymbols["JE"] = LexerTokenType::Operator_Je;
        m_ReservedSymbols["JG"] = LexerTokenType::Operator_Jg;
        m_ReservedSymbols["JL"] = LexerTokenType::Operator_Jl;
    }

    bool Lexer::BuildTokenSequence(const std::string& translationUnit, std::vector<LexerToken>& outputTokens) const
    {
        std::string_view translationUnitView{ translationUnit };
        std::string_view line{};
        std::string_view token{};
        while (Internal::ReadNextLine(translationUnitView, line))
        {
            while (Internal::ReadNextToken(line, token))
            {
                if (token.back() == ':')
                {
                    token.remove_suffix(1);
                    if (Internal::DoesTokenMatchPredicate(token, Internal::IsValidText))
                    {
                        outputTokens.push_back(LexerToken{ LexerTokenType::Label, token });
                    }
                    else
                    {
                        //TODO: error
                        return false;
                    }
                }
                else if (token.front() == '[' && token.back() == ']')
                {
                    const auto foundSymbol{ m_ReservedSymbols.find(std::string{ token }) };
                    if (foundSymbol != m_ReservedSymbols.end())
                    {
                        outputTokens.push_back(LexerToken{ foundSymbol->second, token });
                    }
                    else
                    {
                        //TODO: error
                        return false;
                    }
                }
                else if (Internal::DoesTokenMatchPredicate(token, std::isdigit))
                {
                    outputTokens.push_back(LexerToken{ LexerTokenType::Number, token });
                }
                else if (std::isalpha(token.front()) && Internal::DoesTokenMatchPredicate(token, Internal::IsValidText))
                {
                    const auto foundSymbol{ m_ReservedSymbols.find(std::string{ token }) };
                    LexerTokenType tokenType{ foundSymbol != m_ReservedSymbols.end() ? foundSymbol->second : LexerTokenType::Text };
                    outputTokens.push_back(LexerToken{ tokenType, token });
                }
                else
                {
                    //TODO: error
                    return false;
                }
            }
            if (outputTokens.empty() || outputTokens.back().TokenType != LexerTokenType::Label)
            {
                outputTokens.push_back(LexerToken{ LexerTokenType::OperationEnd });
            }
        }
        return true;
    }
}