#include "characterdisplay.h"

#include <iostream>
#include <Windows.h>

#include "memoryunit.h"

namespace FluffyGamevev::CPU8
{
    void CharacterDisplay::RenderDisplay(const MemoryUnit& ram)
    {
        HANDLE output{ GetStdHandle(STD_OUTPUT_HANDLE) };

        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(output, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(output, &cursorInfo);
        SetConsoleCursorPosition(output, { 0, 0 });

        std::putc('+', stdout);
        for (short i = 0; i < k_ScreenWidth; ++i)
        {
            std::putc('-', stdout);
        }
        std::putc('+', stdout);
        std::putc('\n', stdout);

        for (short j = 0; j < k_ScreenHeight; ++j)
        {
            std::putc('|', stdout);
            for (short i = 0; i < k_ScreenWidth; ++i)
            {
                u8 charAddr{ (u8)(k_VideoDataStart + i + j * k_ScreenWidth) };
                u8 c{ ram.Buffer[charAddr] };
                if (c < 32 || c > 126) { c = ' '; }
                std::putc(c, stdout);
            }
            std::putc('|', stdout);
            std::putc('\n', stdout);
        }

        std::putc('+', stdout);
        for (short i = 0; i < k_ScreenWidth; ++i)
        {
            std::putc('-', stdout);
        }
        std::putc('+', stdout);
        std::putc('\n', stdout);
    }
}