#include <thread>
#include "FtxuiLoop.h"

FtxuiLoop::FtxuiLoop(chip8::emu& emulator, ftxui::ScreenInteractive& scr) : 
    AppLoopBase{},
    component_{ std::make_shared<Chip8Component>(emulator) },
    screen_{&scr}
{}

FtxuiLoop::FtxuiLoop(FtxuiLoop&& other) noexcept : 
    component_{std::move(other.component_)},
    screen_{other.screen_}
{
    other.screen_ = nullptr;
}

FtxuiLoop& FtxuiLoop::operator=(FtxuiLoop&& rhs) noexcept
{
    this->component_ = std::move(rhs.component_);
    this->screen_ = rhs.screen_;
    rhs.screen_ = nullptr;
    return *this;
    // TODO: insert return statement here
}



//FtxuiLoop::~FtxuiLoop() {}

int FtxuiLoop::startLoop() {
    std::thread a_thread{ [this]() {while (true) { screen_->PostEvent(ftxui::Event::Custom); } return 0; } };
    //screen_->PostEvent(ftxui::Event::Custom);
    screen_->Loop(component_);

    //a_thread.join();

    return 0;
}
