#include <memory>
#include "emu.h"
#include "chip8_display.h"

namespace app {
    ftxui::Element chip8_element(chip8::emu& emulator) {
        return std::make_shared<chip8_display>(emulator.renderData());
    }
}
