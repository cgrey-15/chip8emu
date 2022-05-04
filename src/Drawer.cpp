#include <vector>
#include <variant>
//#define NOMINMAX
//#include <Windows.h>
#include "Drawer.h"
#include "emu.h"


void renderer::update() {
		return;
}

void renderer::render() {
		return;
}

using namespace renderer;

void DrawMachine::initialize() {
		consoleWindowHandle = initscr();
	//using namespace tabulate;
	//using some_t = std::variant<std::string, Table>;
	i_ = 0;
	//s = Screen::Create(Dimension::Fixed(64), Dimension::Fixed(32));
	//using Row_t = Table::Row_t;
	//t.format().color(Color::white).border("").corner("").column_separator("").padding(0);

	//std::vector<some_t> blah{ 64, {"."}};

	chtype fun{};
	for (int j = 0; j < 32; ++j) {
		for (int i = 0; i < 64; ++i) {
			//s.PixelAt(i, j).background_color = Color::White;
			
		}
		//t.add_row(blah);
	}
	resize_term(33, 65);
	//keypad(curscr, true);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(3, COLOR_BLACK, COLOR_CYAN);
	init_pair(4, COLOR_WHITE, COLOR_RED);
	init_pair(5, COLOR_WHITE, COLOR_BLUE);

	color_set(0, NULL);
	for (int i = 0; i < 64; ++i) {
		chtype_helper a{};
		a.colorPairIndex = 4;
		a.character = ' ';
		if ((i / 10) % 2 == 1) {
			a.colorPairIndex = 5;
		}
		if (i < 10) {
			a.character = '0' + i;
		}
		else {
			if (i % 10 == 0) {
				a.character = '0';
			}
			else if (i % 5 == 0) {
				a.character = '5';
			}
		}
		mvaddch(32, i, std::bit_cast<chtype>(a));
	}
	for (int i = 0; i < 32; ++i) {
		chtype_helper a{};
		a.colorPairIndex = 4;
		a.character = ' ';
		if ((i / 10) % 2 == 1) {
			a.colorPairIndex = 5;
		}
		if (i < 10) {
			a.character = '0' + i;
		}
		else {
			if (i % 10 == 0) {
				a.character = '0';
			}
			else if (i % 5 == 0) {
				a.character = '5';
			}
		}
		mvaddch(i, 64, std::bit_cast<chtype>(a));
	}
	echo();
	//meta(curscr, false);
	noraw();
	nodelay(consoleWindowHandle, true);
	//s.Print();
}

DrawMachine& renderer::operator<<(std::ostream& stream, DrawMachine& machine)
{

	//stream << machine.t;
	return machine;
	// TODO: insert return statement here
}

void DrawMachine::update() {
	++i_;
	i_ %= 2;
	return;
}

chip8::Key DrawMachine::getKeyPressed() const {
		decltype(getch()) keycode{};

		keycode = getch();

		chip8::Key res{};

		switch(keycode) {
				case '0': res = chip8::Key::Zero;
							break;
				case '1': res = chip8::Key::One;
							break;
				case '2': res = chip8::Key::Two;
							break;
				case '3': res = chip8::Key::Three;
							break;
				case '4': res = chip8::Key::Four;
							break;
				case '5': res = chip8::Key::Five;
							break;
				case '6': res = chip8::Key::Six;
							break;
				case '7': res = chip8::Key::Seven;
							break;
				case '8': res = chip8::Key::Eight;
							break;
				case '9': res = chip8::Key::Nine;
							break;
				case 'a': res = chip8::Key::A;
							break;
				case 'b': res = chip8::Key::B;
							break;
				case 'c': res = chip8::Key::C;
							break;
				case 'd': res = chip8::Key::D;
							break;
				case 'e': res = chip8::Key::E;
							break;
				case 'f': res = chip8::Key::F;
							break;
				default: res = chip8::Key::None;
						 break;
		}
		return res;
}

void DrawMachine::draw(const chip8::emu& emulator) {
	using namespace ftxui;

	auto data = emulator.renderData();

	//const auto poof = SP;

	for (int j = 0; j < decltype(data)::extent; ++j) {
		for (int i = 0; i < data[j].max_size(); ++i) {
			chtype_helper a{};
			a.character = ' ';
			a.colorPairIndex = data[j][i] * 2;
			mvaddch(j, i, std::bit_cast<chtype>(a));
		}
	}

	move(0, 0);

	//auto huh = GetConsoleOutputCP();
	refresh();
	//move(0, 0);

	//st << clearScreen;


	//s.Print();
}
