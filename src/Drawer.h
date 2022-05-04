#ifndef DRAWER_H
#define DRAWER_H
//#include <tabulate/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <curses.h>
//#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <iosfwd>
#include <memory>
#include "chip8.h"

namespace chip8 {
class emu;
}

namespace renderer {

	struct chtype_helper {
		uint32_t character : 16;
		uint32_t modifier : 8;
		uint32_t colorPairIndex : 8;
	};


		class DrawMachine {
				public:
				void initialize();
				void update();
				void draw(const chip8::emu& emulator);
				chip8::Key getKeyPressed() const;
				friend renderer::DrawMachine& operator<<(std::ostream&, DrawMachine&);
				private:
				//tabulate::Table t;
				int i_;
				//std::unique_ptr<WINDOW> consoleWindow{ initscr() };
				WINDOW* consoleWindowHandle{};

				//std::string clearScreen = std::string(static_cast<size_t>(32*66), '\x08');
		};

		DrawMachine& operator<<(std::ostream& stream, DrawMachine& machine);

		void update();
		void render();
}

#endif // DRAWER_H
