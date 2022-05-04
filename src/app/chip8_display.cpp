#include <ftxui/screen/screen.hpp>
#include "chip8_display.h"
chip8_display::chip8_display(screen_data_const_t data) :
	ftxui::Node{}, screen{ data }
{
}
//chip8_display::~chip8_display() {}
#if 1
void chip8_display::ComputeRequirement() {
	// TODO require 64 by 32 cell resolution
	requirement_.min_x = 64;
	requirement_.min_y = 32;

	requirement_.flex_grow_x = 0;
	requirement_.flex_grow_y = 0;

	requirement_.flex_shrink_x = 0;
	requirement_.flex_shrink_y = 0;
}
#endif
void chip8_display::Render(ftxui::Screen& canvas) {
	// TODO "draw" chip8 sprites to canvas screen buffer

	int j = 0;
	int i = 0;
	for (auto& row : screen) {
		j = 0;
		for (auto cell : row) {
			auto& pixel = canvas.PixelAt(j, i);

#if 0
			if(&pixel == reinterpret_cast<ftxui::Pixel*>((void*)0x00007ff769b282c8) ) {
				int hello_ptr = -1;
				int favorite_pixel = 0;
			}
#endif

			if (cell)
				pixel.background_color = ftxui::Color::Cyan;
			else
				pixel.background_color = ftxui::Color::Yellow;

			pixel.character = ' ';
			++j;
		}
		++i;
	}
}
