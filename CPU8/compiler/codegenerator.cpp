#include "codegenerator.h"

#include "../core/memoryunit.h"

namespace FluffyGamevev::CPU8::Compiler
{
    namespace Internal
    {
        bool IsRegister(LexerTokenType tokenType)
        {
            return (u32)tokenType >= (u32)LexerTokenType::RegisterA && (u32)tokenType <= (u32)LexerTokenType::RegisterSP;
        }

        bool IsRegisterPointer(LexerTokenType tokenType)
        {
            return (u32)tokenType >= (u32)LexerTokenType::RegisterPointerA && (u32)tokenType <= (u32)LexerTokenType::RegisterPointerSP;
        }

        u8 ComputeRegisterCode(LexerTokenType tokenType)
        {
            return ((u8)tokenType - (u8)LexerTokenType::RegisterA) & 0x03;
        }

        u8 ComputeRegisterPointerCode(LexerTokenType tokenType)
        {
            return ((u8)tokenType - (u8)LexerTokenType::RegisterPointerA) & 0x03;
        }

        void PushToProgram(u8 byte, MemoryUnit& rom, u8& programSize)
        {
            rom.Buffer[programSize] = byte;
            ++programSize;
        }

        LexerTokenType GetNextToken(TokensIterator& currentIt, TokensIterator endIt)
        {
            LexerTokenType nextToken{ LexerTokenType::Invalid };
            if (currentIt != endIt)
            {
                nextToken = currentIt->TokenType;
                ++currentIt;
            }
            return nextToken;
        }

        bool NopHandler(std::unordered_map<std::string, u8>& constants, TokensIterator& currentIt, TokensIterator endIt,
                        MemoryUnit& rom, u8& programSize)
        {
            PushToProgram(0xf8, rom, programSize);
            return GetNextToken(currentIt, endIt) == LexerTokenType::OperationEnd;
        }

        bool ConstHandler(std::unordered_map<std::string, u8>& constants, TokensIterator& currentIt, TokensIterator endIt,
            MemoryUnit& rom, u8& programSize)
        {
            if (currentIt != endIt && currentIt->TokenType == LexerTokenType::Text)
            {
                std::string token{ currentIt->Token };
                ++currentIt;

                //TODO: check const name doesn't already exist

                if(currentIt != endIt && currentIt->TokenType == LexerTokenType::Number)
                {
                    u8 tokenValue{ (u8)std::atoi(currentIt->Token.data()) };
                    constants[token] = tokenValue;
                    ++currentIt;
                    return GetNextToken(currentIt, endIt) == LexerTokenType::OperationEnd;
                }
            }

            //TODO: error
            return false;
        }

        bool LabelHandler(std::unordered_map<std::string, u8>& constants, TokensIterator& currentIt, TokensIterator endIt,
            MemoryUnit& rom, u8& programSize)
        {
            --currentIt;
            if (currentIt != endIt && currentIt->TokenType == LexerTokenType::Label)
            {
                std::string token{ currentIt->Token };
                ++currentIt;

                //TODO: check const name doesn't already exist

                constants[token] = programSize;
                return true;
            }

            //TODO: error
            return false;
        }

        template<u8 RegisterVariantOpCode, u8 ValueVariantOpCode>
        bool UnaryOperationHandler(std::unordered_map<std::string, u8>& constants, TokensIterator& currentIt, TokensIterator endIt,
            MemoryUnit& rom, u8& programSize)
        {
            if (currentIt != endIt)
            {
                LexerTokenType tokenType{ currentIt->TokenType };
                std::string token{ currentIt->Token };
                ++currentIt;

                if (IsRegister(tokenType))
                {
                    if (RegisterVariantOpCode == 0xff)
                    {
                        //TODO: error
                        return false;
                    }

                    u8 opCode{ (u8)(RegisterVariantOpCode | ComputeRegisterCode(tokenType)) };
                    PushToProgram(opCode, rom, programSize);
                }
                else if (tokenType == LexerTokenType::Text)
                {
                    if (ValueVariantOpCode == 0xff)
                    {
                        //TODO: error
                        return false;
                    }

                    auto foundIt{ constants.find(token) };
                    if (foundIt != constants.end())
                    {
                        PushToProgram(ValueVariantOpCode, rom, programSize);
                        PushToProgram(foundIt->second, rom, programSize);
                    }
                    else
                    {
                        //TODO: error
                        return false;
                    }
                }
                else if (tokenType == LexerTokenType::Number)
                {
                    if (ValueVariantOpCode == 0xff)
                    {
                        //TODO: error
                        return false;
                    }

                    u8 value{ (u8)std::stoi(token) };
                    PushToProgram(ValueVariantOpCode, rom, programSize);
                    PushToProgram(value, rom, programSize);
                }

                return GetNextToken(currentIt, endIt) == LexerTokenType::OperationEnd;
            }

            //TODO: error
            return false;
        }

        template<u8 RegisterVariantOpCode, u8 ValueVariantOpCode>
        bool BinaryOperationHandler(std::unordered_map<std::string, u8>& constants, TokensIterator& currentIt, TokensIterator endIt,
            MemoryUnit& rom, u8& programSize)
        {
            LexerTokenType lhsTokenType{ GetNextToken(currentIt, endIt) };
            if (currentIt != endIt && IsRegister(lhsTokenType))
            {
                LexerTokenType rhsTokenType{ currentIt->TokenType };
                std::string rhsToken{ currentIt->Token };
                ++currentIt;

                if (IsRegister(rhsTokenType))
                {
                    if (RegisterVariantOpCode == 0xff)
                    {
                        //TODO: error
                        return false;
                    }

                    u8 lhsCode{ ComputeRegisterCode(lhsTokenType) };
                    u8 rhsCode{ ComputeRegisterCode(rhsTokenType) };
                    u8 opCode{ (u8)(RegisterVariantOpCode | (lhsCode << 2) | rhsCode) };
                    PushToProgram(opCode, rom, programSize);
                }
                else if (rhsTokenType == LexerTokenType::Text)
                {
                    if (ValueVariantOpCode == 0xff)
                    {
                        //TODO: error
                        return false;
                    }

                    auto foundIt{ constants.find(rhsToken) };
                    if (foundIt != constants.end())
                    {
                        u8 lhsCode{ ComputeRegisterCode(lhsTokenType) };
                        u8 opCode{ (u8)(ValueVariantOpCode | lhsCode) };
                        PushToProgram(opCode, rom, programSize);
                        PushToProgram(foundIt->second, rom, programSize);
                    }
                    else
                    {
                        //TODO: error
                        return false;
                    }
                }
                else if (rhsTokenType == LexerTokenType::Number)
                {
                    if (ValueVariantOpCode == 0xff)
                    {
                        //TODO: error
                        return false;
                    }

                    u8 value{ (u8)std::stoi(rhsToken) };
                    u8 lhsCode{ ComputeRegisterCode(lhsTokenType) };
                    u8 opCode{ (u8)(ValueVariantOpCode | lhsCode) };
                    PushToProgram(opCode, rom, programSize);
                    PushToProgram(value, rom, programSize);
                }

                return GetNextToken(currentIt, endIt) == LexerTokenType::OperationEnd;
            }

            //TODO: error
            return false;
        }

        bool MovOperationHandler(std::unordered_map<std::string, u8>& constants, TokensIterator& currentIt, TokensIterator endIt,
            MemoryUnit& rom, u8& programSize)
        {
            LexerTokenType lhsTokenType{ GetNextToken(currentIt, endIt) };
            bool isLhsRegister{ IsRegister(lhsTokenType) };
            bool isLhsRegisterPointer{ IsRegisterPointer(lhsTokenType) };
            if (currentIt != endIt && (isLhsRegister || isLhsRegisterPointer))
            {
                LexerTokenType rhsTokenType{ currentIt->TokenType };
                std::string rhsToken{ currentIt->Token };
                ++currentIt;

                if (IsRegister(rhsTokenType))
                {
                    u8 baseOpCode{ (u8)(isLhsRegister ? 0x00 : 0x10) };
                    u8 lhsCode{ ComputeRegisterCode(lhsTokenType) };
                    u8 rhsCode{ ComputeRegisterCode(rhsTokenType) };
                    u8 opCode{ (u8)(baseOpCode | (lhsCode << 2) | rhsCode) };
                    PushToProgram(opCode, rom, programSize);
                }
                else if (rhsTokenType == LexerTokenType::Text)
                {
                    u8 baseOpCode{ (u8)(isLhsRegister ? 0xdc : 0xe0) };
                    auto foundIt{ constants.find(rhsToken) };
                    if (foundIt != constants.end())
                    {
                        u8 lhsCode{ ComputeRegisterCode(lhsTokenType) };
                        u8 opCode{ (u8)(baseOpCode | lhsCode) };
                        PushToProgram(opCode, rom, programSize);
                        PushToProgram(foundIt->second, rom, programSize);
                    }
                    else
                    {
                        //TODO: error
                        return false;
                    }
                }
                else if (rhsTokenType == LexerTokenType::Number)
                {
                    u8 baseOpCode{ (u8)(isLhsRegister ? 0x10 : 0xe0) };
                    u8 value{ (u8)std::stoi(rhsToken) };
                    u8 lhsCode{ ComputeRegisterCode(lhsTokenType) };
                    u8 opCode{ (u8)(baseOpCode | lhsCode) };
                    PushToProgram(opCode, rom, programSize);
                    PushToProgram(value, rom, programSize);
                }

                return GetNextToken(currentIt, endIt) == LexerTokenType::OperationEnd;
            }

            //TODO: error
            return false;
        }
    }

    CodeGenerator::CodeGenerator()
    {
        m_CodeGenerators.resize((size_t)LexerTokenType::Count);

        m_CodeGenerators[(size_t)LexerTokenType::Operator_Nop] = Internal::NopHandler;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Const] = Internal::ConstHandler;
        m_CodeGenerators[(size_t)LexerTokenType::Label] = Internal::LabelHandler;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Dat] = Internal::UnaryOperationHandler<0xff, 0xf9>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Mov] = Internal::MovOperationHandler;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Load] = Internal::BinaryOperationHandler<0xec, 0xff>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Push] = Internal::UnaryOperationHandler<0xe4, 0xff>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Pop] = Internal::UnaryOperationHandler<0xe8, 0xff>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Cmp] = Internal::BinaryOperationHandler<0xa0, 0xf0>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Inc] = Internal::UnaryOperationHandler<0xd4, 0xff>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Dec] = Internal::UnaryOperationHandler<0xd8, 0xff>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Not] = Internal::UnaryOperationHandler<0xd0, 0xff>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Add] = Internal::BinaryOperationHandler<0x20, 0xb0>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Sub] = Internal::BinaryOperationHandler<0x30, 0xb4>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Mul] = Internal::BinaryOperationHandler<0x40, 0xb8>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Div] = Internal::BinaryOperationHandler<0x50, 0xbc>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Mod] = Internal::BinaryOperationHandler<0x60, 0xcc>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_And] = Internal::BinaryOperationHandler<0x70, 0xcc>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Or] = Internal::BinaryOperationHandler<0x80, 0xcc>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Xor] = Internal::BinaryOperationHandler<0x90, 0xcc>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Jmp] = Internal::UnaryOperationHandler<0xff, 0xfa>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Je] = Internal::UnaryOperationHandler<0xff, 0xfb>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Jg] = Internal::UnaryOperationHandler<0xff, 0xfc>;
        m_CodeGenerators[(size_t)LexerTokenType::Operator_Jl] = Internal::UnaryOperationHandler<0xff, 0xfd>;
    }

    bool CodeGenerator::CompileProgram(const std::vector<LexerToken>& tokens, MemoryUnit& rom, u8& programSize) const
    {
        std::unordered_map<std::string, u8> constants{};

        auto currentIt{ tokens.begin() };
        auto endIt{ tokens.end() };
        while (currentIt != endIt)
        {
            LexerTokenType currentTokenType{ currentIt->TokenType };
            CodeGeneratorHandler generator{ m_CodeGenerators[(size_t)currentTokenType] };
            ++currentIt;

            if (generator == nullptr || !generator(constants, currentIt, endIt, rom, programSize))
            {
                //TODO: error
                return false;
            }
        }

        //TODO: resolve labels

        return true;
    }
}