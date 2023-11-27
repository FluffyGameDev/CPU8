#pragma once

#include "types.h"

namespace FluffyGamevev::CPU8
{
    struct MemoryUnit;

    class CharacterDisplay
    {
    public:
        void RenderDisplay(const MemoryUnit& ram);

    private:
        static constexpr u8 k_ScreenWidth{ 8 };
        static constexpr u8 k_ScreenHeight{ 8 };
        static constexpr u8 k_VideoDataStart{ 255 - k_ScreenWidth * k_ScreenHeight };
    };
}