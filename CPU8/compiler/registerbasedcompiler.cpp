#include "registerbasedcompiler.h"

#include <string>

#include "../core/memoryunit.h"
#include "parserutils.h"

#include "opparser/noargparser.h"


namespace FluffyGamevev::CPU8
{
    RegisterBasedCompiler::RegisterBasedCompiler()
    {
        m_Parsers[ParserUtils::ConvertTokenToU32("NOP")] = new Parser::NoArgParser(0xf8);
    }

    bool RegisterBasedCompiler::CompileProgram(std::istream& input, MemoryUnit& rom, u8& programSize) const
    {
        std::string line{};
        std::unordered_map<std::string, u8> constants{};
        while (std::getline(input, line))
        {
            std::string_view lineView{ line };
            std::string_view operation{};
            if (ParserUtils::FindNextToken(lineView, operation))
            {
                if (operation.back() == ':')
                {
                    ParserUtils::SkipWhitespaces(lineView);
                    if (lineView.size() > 0)
                    {
                        //error
                        return false;
                    }
                    else
                    {
                        operation.remove_suffix(1);
                        constants[std::string(operation)] = programSize;
                    }
                }
                else if (operation == "CONST")
                {
                    std::string_view constantName{};
                    std::string_view constantValue{};
                    if (ParserUtils::FindNextToken(lineView, constantName) && ParserUtils::FindNextToken(lineView, constantValue))
                    {
                        u8 value{ (u8)std::atoi(constantValue.data()) };
                        constants[std::string(constantName)] = value;
                    }
                    else
                    {
                        //error
                        return false;
                    }
                }
                else
                {
                    u32 parserId{ ParserUtils::ConvertTokenToU32(operation) };
                    const auto foundParserIt{ m_Parsers.find(parserId) };
                    if (foundParserIt != m_Parsers.end())
                    {
                        if (!foundParserIt->second->Parse(lineView, rom, programSize))
                        {
                            //error
                            return false;
                        }
                    }
                    else
                    {
                        //error
                        return false;
                    }
                }
            }
        }

        //TODO: resolve labels

        return true;
    }
}