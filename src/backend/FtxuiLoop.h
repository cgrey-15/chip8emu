#pragma once
#ifndef FTXUI_LOOP_H
#define FTXUI_LOOP_H

#include "app/chip8_component.h"
#include "AppLoop.h"
#include "ftxui/component/screen_interactive.hpp"

namespace chip8 {
    class emu;
}

class FtxuiLoop : public the::AppLoopBase {
public:
    FtxuiLoop(chip8::emu& emulator, ftxui::ScreenInteractive& scr);
    FtxuiLoop(FtxuiLoop&& other) noexcept;

    FtxuiLoop& operator=(FtxuiLoop&& rhs) noexcept;

    ~FtxuiLoop() = default;

    int startLoop() override;

private:
    ftxui::Component       component_;
    ftxui::ScreenInteractive* screen_;
};

#endif // FTXUI_LOOP_H
