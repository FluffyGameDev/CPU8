#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "../core/types.h"

namespace FluffyGamevev::CPU8::Compiler
{
    enum class LexerTokenType
    {
        RegisterA,
        RegisterB,
        RegisterC,
        RegisterSP,

        RegisterPointerA,
        RegisterPointerB,
        RegisterPointerC,
        RegisterPointerSP,

        Number,
        Text,
        Label,
        OperationEnd,

        Operator_Nop,
        Operator_Const,
        Operator_Dat,
        Operator_Mov,
        Operator_Load,
        Operator_Push,
        Operator_Pop,
        Operator_Cmp,
        Operator_Inc,
        Operator_Dec,
        Operator_Add,
        Operator_Sub,
        Operator_Mul,
        Operator_Div,
        Operator_Mod,
        Operator_And,
        Operator_Or,
        Operator_Xor,
        Operator_Not,
        Operator_Jmp,
        Operator_Je,
        Operator_Jg,
        Operator_Jl,

        Count,
        Invalid
    };

    struct LexerToken
    {
        LexerTokenType TokenType;
        std::string_view Token;
    };

    class Lexer
    {
    public:
        Lexer();

        bool BuildTokenSequence(const std::string& translationUnit, std::vector<LexerToken>& outputTokens) const;

    private:
        std::unordered_map<std::string, LexerTokenType> m_ReservedSymbols;
    };
}