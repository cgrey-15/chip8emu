#include <bit>
#include <type_traits>
#include <memory>
#include <random>
#include <limits>
#include <cassert>
#include "emu.h"
#include "chip8_priv.hpp"
#include "JustFileThings.h"

namespace {
    constexpr uint16_t CHIP8_INITIAL_PROGRAM_START = 0x200;

    constexpr int CHIP8_FONTSET_SIZE = 80;

    constexpr std::span<const std::uint8_t, CHIP8_FONTSET_SIZE> fnt_{ chip8::priv::fontset.begin(), chip8::priv::fontset.end() };

    std::uint16_t fromBigEndian(const std::uint8_t* data) {
        const std::array<uint8_t, 2> bigERaw = { data[1], data[0] };
        static_assert(sizeof(bigERaw) == sizeof(uint16_t));
        return *std::bit_cast<uint16_t*>(bigERaw.data());
    }

    std::random_device rand_dev{};

    std::uniform_int_distribution<std::uint16_t> rand_vals{ 0x0, 0xff };
    
}

chip8::emu::emu() :
    fontData{ memory.cbegin(), memory.cbegin() + CHIP8_FONTSET_SIZE },
    pc{ CHIP8_INITIAL_PROGRAM_START },
    opcode{},
    regI{},
    regV{},
    sp{},
    delayTimer{},
    soundTimer{},
    key{},
    drawFlag{},
    keyPressed{ Key::None },
    gfx{}
{
    int i = 0;
    for (auto byte : fnt_) {
        memory[i] = byte;
        ++i;
    }
}

void chip8::emu::initialize() {

#ifdef _DEBUG
    std::uninitialized_fill(memory.begin() + CHIP8_INITIAL_PROGRAM_START, memory.end(), 0xff);
#endif
    std::uninitialized_fill(gfx.data(), gfx.data() + (gfx.rowLength() * gfx.colLength()), 0x00);
    for (auto& regist : regV) {
        regist = 0;
    }
    for (auto& k : key) {
        k = 0;
    }

    pc = CHIP8_INITIAL_PROGRAM_START;

    opcode = {};
    regI = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = false;
}

bool chip8::emu::loadGame(const std::uint8_t* data, const size_t size) {

    return loadGame({ data, size });

}

bool chip8::emu::loadGame(const std::span<const std::uint8_t, std::dynamic_extent> data) {
    bool res{ true };
    if (data.size() < memory.size() - CHIP8_INITIAL_PROGRAM_START) {
        size_t i{ CHIP8_INITIAL_PROGRAM_START };
        for (const auto& byte : data) {
            memory[i++] = byte;
        }
    }
    else {
        res = false;
    }
    return res;
}

static int global_i = 0;

chip8::emu::ErrorTuple chip8::emu::emulateCycle() {
    using std::bit_cast;

    ErrorCode err_code = ErrorCode::None;
    ErrorTuple err_tup = { ErrorCode::None, {} };

    opcode = *bit_cast<opcode_t*>(&memory[pc]);//memory[pc] << 8 | memory[pc+1];
    std::basic_string_view<uint8_t> rawOpcode{ &memory[pc], 4 };

    switch (auto font_start = fontData.data() - memory.data(); opcode.opStart) {
    case 0x0:
        // only interprets 0XX(0|E), please fix
        switch (fromBigEndian(&memory[pc]) & 0x0fff) {
        case 0x0e0:
            // clear screen
            std::uninitialized_fill_n(gfx.data(), (gfx.rowLength() * gfx.colLength()), 0);
            break;
        case 0x0ee:
            // return from subroutine
            pc = stack[--sp];
            break;
        default:
            // call 1802 instruction (no op)
            break;
        }
        pc += 2;
        break;
    case 0x1:
        pc = fromBigEndian(&memory[pc]) & 0x0FFF;
        break;
    case 0x2:
        stack[sp] = pc;
        ++sp;
        pc = fromBigEndian(&memory[pc]) & 0x0FFF;
        break;
    case 0x3:
        if (regV[opcode.argA] == memory[pc + 1])
            pc += 2;
        pc += 2;
        break;
    case 0x4:
        if (regV[opcode.argA] != memory[pc + 1])
            pc += 2;
        pc += 2;
        break;
    case 0x5:
        switch (opcode.opMisc) {
        case 0:
            if (regV[opcode.argA] == regV[opcode.argB])
                pc += 2;
            break;
        default:
            err_tup = { ErrorCode::UnsupportedInstruction, {{memory[pc], memory[pc + 1]}} };
        }
        pc += 2;
        break;
    case 0x6:
        regV[opcode.argA] = memory[pc + 1];
        pc += 2;
        break;
    case 0x7:
        regV[opcode.argA] += memory[pc + 1];
        pc += 2;
        break;
    case 0x8:
        switch (opcode.opMisc) {
        case 0x0:
            regV[opcode.argA] = regV[opcode.argB];
            pc += 2;
            break;
        case 0x1:
            regV[opcode.argA] |= regV[opcode.argB];
            pc += 2;
            break;
        case 0x2:
            regV[opcode.argA] &= regV[opcode.argB];
            pc += 2;
            break;
        case 0x3:
            regV[opcode.argA] ^= regV[opcode.argB];
            pc += 2;
            break;
        case 0x4: {
            uint16_t overflow = 0x01 & ((regV[opcode.argA] + regV[opcode.argB]) >> 8); // TODO review this
            regV[opcode.argA] += regV[opcode.argB];
            regV[0xf] = overflow;
            pc += 2;
        }
            break;
        case 0x5: {
            auto carry_bit = (!(regV[opcode.argB] > regV[opcode.argA]));
            regV[opcode.argA] -= regV[opcode.argB];
            regV[0xf] = carry_bit;
            pc += 2;
            break;
        }
        case 0x6:
            regV[0xf] = regV[opcode.argA] & 0x01;
            regV[opcode.argA] >>= 1;
            pc += 2;
            break;
        case 0x7: {
            //regV[0xf] = 0;
            auto is_carry = regV[opcode.argB] < regV[opcode.argA];
            regV[opcode.argA] = regV[opcode.argB] - regV[opcode.argA];
            regV[0xf] = !is_carry;
        }
            pc += 2;
            break;
        case 0xE:
            regV[0xf] = regV[opcode.argA] >> ((sizeof(regV[0]) * 8) - 1);
            regV[opcode.argA] <<= 1;
            pc += 2;
            break;
        default:
            err_tup = { ErrorCode::UnsupportedInstruction, {{memory[pc], memory[pc + 1]}} };
        }
        //switch(opcode & 0x000F) {
        //		case 
        //}
        break;
    case 0x9:
        switch (opcode.opMisc) {
        case 0:
            if (regV[opcode.argA] != regV[opcode.argB])
                pc += 2;
            break;
        default:
            err_tup = { ErrorCode::UnsupportedInstruction, {{memory[pc], memory[pc + 1]}} };
        }
        pc += 2;
        break;
    case 0xA:
        regI = fromBigEndian(&memory[pc]) & 0x0FFF;
        pc += 2;
        break;
    case 0xB:
        pc = (fromBigEndian(&memory[pc]) & 0x0FFF) + regV[0];
        break;
    case 0xC:
        regV[opcode.argA] = rand_vals(rand_dev) & memory[pc + 1];
        pc += 2;
        break;
    case 0xD:
    {
        //uint16_t pixel;
        auto dcoord_x = regV[opcode.argA];// % 64;
        auto dcoord_y = regV[opcode.argB];// % 32;
        uint16_t sprite_height = opcode.opMisc;
        regV[0xf] = priv::draw_sprite(gfx, &memory[regI], &memory[regI + sprite_height], dcoord_x, dcoord_y, true);
#if 0
        regV[0xf] = 0;
        for (int y_line_sprite = 0; y_line_sprite < sprite_height; ++y_line_sprite) {
            pixel = memory[regI + y_line_sprite];
            for (int x_col_sprite = 0; x_col_sprite < 8; ++x_col_sprite) {
                if ((pixel & (0x80 >> x_col_sprite)) != 0) {
                    if (gfx[(y_line_sprite+dcoord_y)%32][(x_col_sprite+dcoord_x)%64] == 1) {
                        regV[0xF] = 1;
                    }
                    gfx[(y_line_sprite+dcoord_y)%32][(x_col_sprite+dcoord_x)%64] ^= 1;
                }
            }
        }
#endif
    }
        drawFlag = true;
        pc += 2;

        break;
    case 0xE:
        switch (memory[pc + 1]) {
        case 0x9E:
            if (key[regV[opcode.argA]]) {
                pc += 2;
            }
            break;
        case 0xA1:
            if (!key[regV[opcode.argA]]) {
                pc += 2;
            }
            break;
        default:
            err_tup = { ErrorCode::UnsupportedInstruction, {{memory[pc], memory[pc + 1]}} };
            break;
        }
        pc += 2;
        break;
    case 0xF:
        switch (static_cast<uint8_t>(fromBigEndian(&memory[pc]))) {
        case 0x07:
            regV[opcode.argA] = delayTimer;
            pc += 2;
            break;
        case 0x0A:
            if (keyPressed != chip8::Key::None) {
                uint8_t rawVal = static_cast<uint8_t>(keyPressed);
                //uint8_t finalKey = rawVal > 9 ? rawVal+32 : keyPressed;
                //regV[opcode.argA] = rawVal > 9 ? 'A'-10+rawVal : '0'+rawVal; //finalKey;
                regV[opcode.argA] = rawVal;
                pc += 2;
            }
            break;
        case 0x15:
            delayTimer = regV[opcode.argA];
            pc += 2;
            ++global_i;
            break;
        case 0x18:
            soundTimer = regV[opcode.argA];
            pc += 2;
            break;
        case 0x1E:
            //if (opcode.argA != 0xf)
                regI += regV[opcode.argA];
            pc += 2;
            break;
        case 0x29:
            regI = font_start + (regV[opcode.argA] * 5);
            pc += 2;
            break;
        case 0x33:
            // MAYBE change this...
            memory[regI] = (regV[opcode.argA] / 10) / 10;
            memory[regI + 1] = (regV[opcode.argA] / 10) % 10;
            memory[regI + 2] = regV[opcode.argA] % 10;
            pc += 2;
            break;
        case 0x55:
            std::uninitialized_copy_n(&regV[0], opcode.argA + 1, &memory[regI]);
            //regI += opcode.argA + 1;
            pc += 2;
            break;
        case 0x65:
            static_assert(sizeof(memory[0]) == sizeof(regV[0]));
            std::uninitialized_copy_n(&memory[regI], opcode.argA + 1, &regV[0]);
            //regI += opcode.argA + 1;
            pc += 2;
            break;
            /**
        case 0x75:
            for(int i = 0; i < opcode.argA; ++i) {
                    hp48Flag[i] = regV[i];
            }
            pc += 2;
            break;
        case 0x85:
            for(int i = 0; i < opcode.argA; ++i) {
                    regV[i] = hp48Flag[i];
            }
            pc += 2;
            break;**/
        default:
            uint8_t val1 = static_cast<uint8_t>(bit_cast<uint16_t>(opcode));
            uint8_t val_correct = static_cast<uint8_t>(fromBigEndian(&memory[pc]));
            err_tup = { ErrorCode::UnsupportedInstruction, {{memory[pc], memory[pc + 1]}} };
            //std::exit(-55);
        }
        break;
    default:
        err_tup = { ErrorCode::UnsupportedInstruction, {{memory[pc], memory[pc + 1]}} };
        //std::exit(-15);
    }

    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0) {
        if (soundTimer == 1) {
            // do something...
            --soundTimer;
        }
    }
    //global_i++;
    return err_tup;
}

void chip8::emu::setKeys() {
    return;
}

void chip8::emu::setKey(const Key chip8Keycode, const bool isPressed) {
    auto index = static_cast<int>(chip8Keycode);
    if (isPressed) {
        key.back() = 0;
        keyPressed = !key[index] ? chip8Keycode : keyPressed;
        key[index] = isPressed;
    }
    else {
        key[index] = isPressed;
#if 1
        bool abutton_pressed = false;
        for (auto it = key.cbegin(), end = key.cend() - 1; it != end; ++it) {
            abutton_pressed = abutton_pressed || *it;
        }
        keyPressed = !abutton_pressed ? Key::None : keyPressed;
#else
#endif
    }

    //auto* p = key.data();
    //std::span<uint8_t, 16> myData{key.data(), 16};

    //int i = 0;
}

std::span<const std::array<const uint8_t, chip8::emu::GFX_WIDTH>, chip8::emu::GFX_HEIGHT> chip8::emu::renderData() const {
    using return_t = decltype(renderData());
    return return_t{ reinterpret_cast<const return_t::value_type*>(gfx.cdata()), GFX_HEIGHT };
}
namespace chip8 {
    namespace priv {
        template<typename It>
        inline uint16_t get_pixels(It it, bool use_full_space) {
            uint16_t value;
            if (use_full_space) {
                if constexpr (std::endian::native == std::endian::big) {
                    value = *std::bit_cast<uint16_t*>(it);
                }
                else {
                    value = (*(it + 0) << 8) | *(it+1);
                }
            }
            else {
                value = *it;
            }
            return value;
        }
    }
}

bool chip8::priv::draw_sprite(emu::gfx_buffer_t& canvas, const uint8_t* data_begin, const uint8_t* data_end, const uint8_t x, const uint8_t y, bool s_mode)
{
    uint16_t pixel;
    bool collision = false;
    if (!s_mode) {
        for (auto it = data_begin; it < data_end; ++it) {

            pixel = *it;
            for (int sprite_col = 0; sprite_col < 8; ++sprite_col) {

                if ((pixel & (0x80 >> sprite_col)) != 0) {
                    auto sprite_row = it - data_begin;
                    if (canvas[((sprite_row*2) + (y*2)) % emu::GFX_HEIGHT][((sprite_col*2) + (x*2)) % emu::GFX_WIDTH] == 1)
                        collision = true;
                    canvas[((sprite_row*2) + (y*2)) % emu::GFX_HEIGHT][((sprite_col*2) + (x*2)) % emu::GFX_WIDTH] ^= 1;
                    canvas[((sprite_row*2) + (y*2)) % emu::GFX_HEIGHT][((sprite_col*2) + (x*2)+1) % emu::GFX_WIDTH] ^= 1;
                    canvas[((sprite_row*2) + (y*2)+1) % emu::GFX_HEIGHT][((sprite_col*2) + (x*2)) % emu::GFX_WIDTH] ^= 1;
                    canvas[((sprite_row*2) + (y*2)+1) % emu::GFX_HEIGHT][((sprite_col*2) + (x*2)+1) % emu::GFX_WIDTH] ^= 1;
                }
            }
        }
    }
    else {
        const uint8_t* end_prime = nullptr;
        if (data_begin == data_end) {
            end_prime = data_begin + (16*2);
        }
        else {
            end_prime = data_end;
        }
        for (auto it = data_begin; it < end_prime; ++it) {

            int sprite_width = data_begin != data_end ? 8 : 16;
            uint16_t bit_index = data_begin != data_end ? 0x80 : 0x8000;

            pixel = get_pixels(it, data_begin == data_end);
            it += (data_begin == data_end);
            for (int sprite_col = 0; sprite_col < sprite_width; ++sprite_col) {

                if ((pixel & (bit_index >> sprite_col)) != 0) {
                    auto sprite_row = (it - data_begin)/(data_begin==data_end ? 2: 1);
                    if (canvas[(sprite_row + y) % emu::GFX_HEIGHT][(sprite_col + x) % emu::GFX_WIDTH] == 1)
                        collision = true;
                    canvas[(sprite_row + y) % emu::GFX_HEIGHT][(sprite_col + x) % emu::GFX_WIDTH] ^= 1;
                }
            }
        }
    }
    return collision;
}

const std::array<uint8_t, 80> chip8::emu::fontset =
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
