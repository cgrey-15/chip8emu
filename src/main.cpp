#include<iostream>
#include<vector>
#include<chrono>

#ifdef __cpp_lib_format
#include<format>
#else
#include<fmt/format.h>
#endif

#include "main.h"
#include "JustFileThings.h"
#include "emu.h"
#include "AppLoop.h"

#if defined(CHIP8EMU_USE_BACKEND_FTXUI)
#include "backend/FtxuiLoop.h"
#elif defined(CHIP8EMU_USE_BACKEND_CURSES)
#include "Drawer.h"
#else
#include "backend/SfmlLoop.h"
#endif

constexpr auto getArray() {
	constexpr auto size = 48 * 66;
	std::array<char, size + 1> arr{};
	for (int i = 0; i < size + 1; ++i) {
		arr[i] = '\x08';
	}
	arr[size] = '\0';
	return arr;
}

template <unsigned int STEPS>
constexpr float const_delta = 1.f / STEPS;

int main(int argc, const char* argv[]) {

	MoreFileThings::ErrorCode err_co = MoreFileThings::ErrorCode::NoError;

	if (argc == 1) {
		std::cout << "No CHIP-8 rom loaded.\n";
	}
	else if (auto rom = MoreFileThings::open_rom(argv[1]); rom.data) {
		chip8::emu myChip8{};

		setupGraphics();
		setupInput();


		myChip8.initialize();

		if (myChip8.loadGame(*rom.data)) {


#ifdef CHIP8EMU_USE_BACKEND_FTXUI
			auto chip8Screen = ftxui::ScreenInteractive::FitComponent();//FixedSize(64, 32);
			std::unique_ptr<the::AppLoopBase> loopy = std::make_unique<FtxuiLoop>(myChip8, chip8Screen);
#else
			sf::RenderWindow window{ sf::VideoMode{1365, 1024}, "My CHIP-8 Window"};
			//window.setFramerateLimit(480);
			std::unique_ptr<the::AppLoopBase> loopy = std::make_unique<the::SfmlLoop>(myChip8, window, 480.f);
#endif


			loopy->startLoop();
#if 0
			chip8::emu::ErrorTuple res{};

			auto last_time_point = std::chrono::steady_clock::now();
			float acc = 0.f;

			bool someCont = false;
			for (res = myChip8.emulateCycle(); res.first == chip8::emu::ErrorCode::None;) {
				using std::chrono::steady_clock;
				const float whatinterval = const_delta<100800>;
				auto latest_time = steady_clock::now();
				std::chrono::duration<float> time_elapsed = latest_time - last_time_point;
				acc += time_elapsed.count();
				last_time_point = latest_time;

				if (myChip8.isReadyDraw()) {
					heyN.draw(myChip8);
					//drawGraphics();
				}

				//heyN.update();
				//std::cout << heyN;
				//std::cout << clearScreen;



				if (acc >= const_delta<60 * 15> || someCont) {

					myChip8.setKeys();
					auto key = heyN.getKeyPressed();
					myChip8.setKey(key);

					res = myChip8.emulateCycle();
					acc = 0.f;
				}
				//auto test_time_point = steady_clock::now();
				//std::chrono::duration<float> test_timeelapse = test_time_point-latest_time;
				//auto what_now = test_timeelapse.count();
				//int imi = 0;
			}

			std::string error_msg{};

#ifdef __cpp_lib_format
			using std::format;
#else
			using fmt::format;
#endif
			switch (res.first) {
			case chip8::emu::ErrorCode::ProgramEnded: break;
			case chip8::emu::ErrorCode::Other: error_msg = "Unknown error";
				break;
			case chip8::emu::ErrorCode::UnsupportedInstruction:
				error_msg = format("Invalid/unsupported instr: 0x{:02x}{:02x}", (*res.second)[0], (*res.second)[1]);
				break;
			default: error_msg = "No problem apparently";
				break;
			}

			if (!error_msg.empty()) {
				std::cerr << error_msg << std::endl;
			}
#endif
		}
		else {
			std::cout << "Huh?\n";
		}

		std::cout << "Hello me!\n";
	}
	else {
		err_co = rom.result;
		std::cout << "ROM could not be opened. File doesn't exist here/there?\n";
	}

	return static_cast<int>(err_co);
}

void setupGraphics() {
	return;
}
void setupInput() {
	return;
}
void drawGraphics() {
	return;
}

