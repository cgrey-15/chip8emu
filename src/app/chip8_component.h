#ifndef CHIP8_COMPONENT_H
#define CHIP8_COMPONENT_H
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/node.hpp>

namespace chip8{
	class emu;
}

class Chip8Component : public ftxui::ComponentBase {
public:
	Chip8Component(chip8::emu& emulator);
	~Chip8Component() = default;

	ftxui::Element Render() override;

	bool OnEvent(ftxui::Event ev) override;

private:
	chip8::emu* emu_;
};

#endif // CHIP8_COMPONENT_H