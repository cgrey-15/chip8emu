#pragma once
#ifndef CHIP8EMU_SFML_LOOP
#define CHIP8EMU_SFML_LOOP

#include <optional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include "MultiArray.hpp"
#include "emu.h"
#include "AppLoop.h"
#include "util/Timer.hpp"


namespace the {
    class SfmlLoop : public the::AppLoopBase {
    public:
        SfmlLoop(chip8::emu& emulator, sf::RenderWindow& window, float fps = 60.f);
        ~SfmlLoop() = default;

        SfmlLoop(SfmlLoop&&)=delete;
        SfmlLoop& operator=(SfmlLoop&&)=delete;

        int startLoop() override;
    private:
        void getInput();
        chip8::emu::ErrorTuple update();
        void draw();

        chip8::emu*     emulatorPtr_;
        sf::RenderWindow* windowPtr_;
        sf::Texture   canvasTexture_;
        sf::Font font_;
        std::optional<sf::Text> fpsText_;
        std::array<char, 16> fpsTextBuffer_;
        sf::Time prevTime_;
        std::optional<sf::Clock> clock_;
        MultiArray<sf::Uint32, chip8::emu::GFX_HEIGHT, chip8::emu::GFX_WIDTH> canvas_;
        SimpleTimer timer_;
    };
}

#endif