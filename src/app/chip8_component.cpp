#include <ftxui/component/event.hpp>
#include "chip8_component.h"
#include "chip8_display.h"
#include "emu.h"

Chip8Component::Chip8Component(chip8::emu& emulator) :
	ftxui::ComponentBase{}, emu_{ &emulator } {}

ftxui::Element Chip8Component::Render() {
	emu_->emulateCycle();
	return app::chip8_element(*emu_);
}

bool Chip8Component::OnEvent(ftxui::Event ev) {
	return false;
}