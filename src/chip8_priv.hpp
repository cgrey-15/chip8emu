#pragma
#ifndef CHIP8_PRIV_HPP
#define CHIP8_PRIV_HPP
#include <array>
#include <cstdint>
#include "MultiArray.hpp"
#include "emu.h"

//static draw_sprite();

namespace chip8 {
    namespace priv {
        static bool draw_sprite(emu::gfx_buffer_t& canvas, const uint8_t* data_begin, const uint8_t* data_end, const uint8_t x, const uint8_t y, bool s_mode);

        constexpr std::array<uint8_t, 80> fontset =
        { 0xf0, 0x90, 0x90, 0x90, 0xf0,
          0x20, 0x60, 0x20, 0x20, 0x70,
          0xf0, 0x10, 0xf0, 0x80, 0xf0,
          0xf0, 0x10, 0xf0, 0x10, 0xf0,

          0x90, 0x90, 0xf0, 0x10, 0x10,
          0xf0, 0x80, 0xf0, 0x10, 0xf0,
          0xf0, 0x80, 0xf0, 0x90, 0xf0,
          0xf0, 0x10, 0x20, 0x40, 0x40,

          0xf0, 0x90, 0xf0, 0x90, 0xf0,
          0xf0, 0x90, 0xf0, 0x10, 0xf0,
          0xf0, 0x90, 0xf0, 0x90, 0x90,
          0xe0, 0x90, 0xe0, 0x90, 0xe0,

          0xf0, 0x80, 0x80, 0x80, 0xf0,
          0xe0, 0x90, 0x90, 0x90, 0xe0,
          0xf0, 0x80, 0xf0, 0x80, 0xf0,
          0xf0, 0x80, 0xf0, 0x80, 0x80
        };
    }
}

#endif // CHIP8_PRIV_HPP
