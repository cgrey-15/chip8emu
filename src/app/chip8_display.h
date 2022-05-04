#include <span>
#include <array>
#include <cstdint>
#include <ftxui/dom/node.hpp>

// implementation

class chip8_display : public ftxui::Node {
public:
    using screen_data_const_t = std::span<const std::array<const uint8_t, 64>, 32>;
    chip8_display(screen_data_const_t data);
    ~chip8_display()=default;
    void ComputeRequirement() override;
    void Render(ftxui::Screen& canvas) override;

private:
    const screen_data_const_t screen;
};

namespace chip8 {
    class emu;
}
namespace app {
    ftxui::Element chip8_element(chip8::emu& emulator);
}
