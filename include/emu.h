#ifndef CHIP8_EMU_H
#define CHIP8_EMU_H
#include <cstdint>
#include <span>
#include <array>
#include <tuple>
#include <optional>
#include "MultiArray.hpp"
#include "chip8.h"


namespace chip8 {
    class emu {
    public:
        static constexpr size_t GFX_WIDTH = 128;
        static constexpr size_t GFX_HEIGHT = 64;
        using gfx_buffer_t = MultiArray<uint8_t, GFX_HEIGHT, GFX_WIDTH>;
        emu();

        enum class ErrorCode { None, Other, ProgramEnded, UnsupportedInstruction };

        using ErrorTuple = std::pair< ErrorCode, std::optional<std::array<std::uint8_t, 2>>>;
        ErrorTuple emulateCycle();

        inline bool isReadyDraw() const { return drawFlag; }
        void setKeys();
        void setKey(const chip8::Key k, const bool isPressed);

        void initialize();
        bool loadGame(const std::uint8_t* data, const size_t size);
        bool loadGame(const std::span<const std::uint8_t, std::dynamic_extent> data);
        std::span<const std::array<const uint8_t, GFX_WIDTH>, GFX_HEIGHT> renderData() const;

    private:


        struct opcode_t {
            std::uint8_t argA : 4;
            std::uint8_t opStart : 4;
            std::uint8_t opMisc : 4;
            std::uint8_t argB : 4;
        };
        opcode_t opcode;

        std::array <std::uint8_t, 4096> memory;

        gfx_buffer_t gfx;

        std::array<std::uint8_t, 16> regV;
        std::array<std::uint8_t, 8> hp48Flag;
        std::uint16_t regI;
        std::uint16_t pc;

        std::uint8_t delayTimer;
        std::uint8_t soundTimer;

        std::array<std::uint16_t, 16> stack;
        std::uint16_t sp;

        std::array<std::uint8_t, 17> key;
        Key keyPressed;

        bool drawFlag;

        const std::span<const std::uint8_t, 80> fontData;
        static const std::array<uint8_t, 80> fontset;
    };
}
#endif // CHIP8_EMU_H
