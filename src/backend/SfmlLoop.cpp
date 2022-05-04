#ifdef __cpp_lib_format
#include <format>
#else
#include <fmt/format.h>
#endif

#ifdef _DEBUG
#include <iostream>
#endif

#include <cassert>
#include <array>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Audio.hpp>

#include "SfmlLoop.h"
static void resolveEvents(sf::RenderWindow& wdw);

the::SfmlLoop::SfmlLoop(chip8::emu& emulator, sf::RenderWindow& window, float fps) :
    AppLoopBase{},
    emulatorPtr_{ &emulator },
    windowPtr_{ &window },
    canvasTexture_{},
    font_{},
    fpsText_{ std::nullopt },
    fpsTextBuffer_{},
    clock_{ std::nullopt },
    canvas_{},
    timer_{ 1.f / fps }
{
    auto resCanvas = canvasTexture_.create(chip8::emu::GFX_WIDTH, chip8::emu::GFX_HEIGHT);

    if (!resCanvas) {
        std::exit(-99);
    }
    if (font_.loadFromFile("OpenSans-Regular.ttf")) {
        fpsText_.emplace();
        fpsText_->setCharacterSize(20);
        //fpsText_->move(1280.f, 960.f);

        fpsText_->setFont(font_);
        fpsText_->setColor(sf::Color::Green);
        fpsText_->setFillColor(sf::Color::Green);
        fpsText_->setStyle(sf::Text::Regular);

        clock_.emplace();
    }
#ifdef _DEBUG
    else {
        std::cerr << "Uh-oh...\n";
    }
#endif
}

#if 0
the::SfmlLoop::SfmlLoop(SfmlLoop&& other) noexcept :
    //the::AppLoopBase{std::move(other)},
    emulatorPtr_{ other.emulatorPtr_ },
    windowPtr_{ other.windowPtr_ }
    //rawScreen_{}
{
    other.emulatorPtr_ = nullptr;
    other.windowPtr_ = nullptr;
}
#endif

#if 0
the::SfmlLoop& the::SfmlLoop::operator=(SfmlLoop&& rhs) noexcept
{
    //this->AppLoopBase::operator=(std::move(rhs));
    this->emulatorPtr_ = rhs.emulatorPtr_;
    this->windowPtr_ = rhs.windowPtr_;
    rhs.emulatorPtr_ = nullptr;
    rhs.windowPtr_ = nullptr;
    return *this;
}
#endif


int the::SfmlLoop::startLoop()
{
    prevTime_ = clock_ ? clock_->getElapsedTime() : sf::Time{};

    timer_.markPoint();

    while (windowPtr_->isOpen()) {

        resolveEvents(*windowPtr_);

        if (timer_.intervalElapsed()) {
            using namespace std::chrono_literals;
            auto start_time = std::chrono::steady_clock::now();
            auto res = update();

            if (res.first == chip8::emu::ErrorCode::UnsupportedInstruction) {
#ifdef _DEBUG
#if __cpp_lib_format
                using std::format;
#else
                using fmt::format;
#endif
                std::cerr << format("Invalid/unsupported instr: 0x{:02x}{:02x}", (*res.second)[0], (*res.second)[1]);
#endif
                while (res.first == chip8::emu::ErrorCode::UnsupportedInstruction) {
                    resolveEvents(*windowPtr_);
                    if (timer_.intervalElapsed() || true) {
                        draw();
                        res = update();
                    }
                }
#ifdef _DEBUG
                std::cout << "Resuming emulation loop.\n";
#endif
            }
            draw();
            getInput();
            timer_.nextInterval();
            auto elapsed_time = std::chrono::steady_clock::now() - start_time;
            auto used_time = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<long long, std::ratio<1, 480>>{ 1 }) - elapsed_time;
            used_time = std::max(used_time, 0ns);
            used_time = (used_time * 88)/100;
            sf::sleep(sf::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(used_time).count()));

        }

        timer_.markPoint();
    }
    return 0;
}
chip8::emu::ErrorTuple the::SfmlLoop::update()
{
    return emulatorPtr_->emulateCycle();
}

void the::SfmlLoop::draw()
{


    if (emulatorPtr_->isReadyDraw()) {
        auto source = emulatorPtr_->renderData();
        int i = 0;
        for (auto& row : source) {
            int j = 0;
            for (auto col : row) {
                canvas_[i][j] = 0xff000000 | (-1 * col);
                ++j;
            }
            ++i;
        }
        canvasTexture_.update(reinterpret_cast<const sf::Uint8*>(canvas_.cdata()));
    }

    windowPtr_->clear(sf::Color::Black);
    sf::Sprite the_sprite{ canvasTexture_ };
    the_sprite.scale(10.f, 10.f);

    windowPtr_->draw(the_sprite);

    if (fpsText_) {
#ifdef __cpp_lib_format
        using std::format_to;
#else
        using fmt::format_to;
#endif
        float fps;
        sf::Time currTime;

        currTime = clock_->getElapsedTime();
        fps = 1.f / (currTime.asSeconds() - prevTime_.asSeconds());

        *(format_to(fpsTextBuffer_.begin(), "{: >4.2f}", fps)) = '\0';
        fpsText_->setString(fpsTextBuffer_.data());
        prevTime_ = currTime;

        windowPtr_->draw(*fpsText_);
    }
    windowPtr_->display();
}

void the::SfmlLoop::getInput()
{
    using namespace sf;

    if (Keyboard::isKeyPressed(Keyboard::Key::X))
        emulatorPtr_->setKey(chip8::Key::Zero, true);
    else
        emulatorPtr_->setKey(chip8::Key::Zero, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::Num1))
        emulatorPtr_->setKey(chip8::Key::One, true);
    else
        emulatorPtr_->setKey(chip8::Key::One, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::Num2))
        emulatorPtr_->setKey(chip8::Key::Two, true);
    else
        emulatorPtr_->setKey(chip8::Key::Two, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::Num3))
        emulatorPtr_->setKey(chip8::Key::Three, true);
    else
        emulatorPtr_->setKey(chip8::Key::Three, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::Q))
        emulatorPtr_->setKey(chip8::Key::Four, true);
    else
        emulatorPtr_->setKey(chip8::Key::Four, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::W))
        emulatorPtr_->setKey(chip8::Key::Five, true);
    else
        emulatorPtr_->setKey(chip8::Key::Five, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::E))
        emulatorPtr_->setKey(chip8::Key::Six, true);
    else
        emulatorPtr_->setKey(chip8::Key::Six, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::A))
        emulatorPtr_->setKey(chip8::Key::Seven, true);
    else
        emulatorPtr_->setKey(chip8::Key::Seven, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::S))
        emulatorPtr_->setKey(chip8::Key::Eight, true);
    else
        emulatorPtr_->setKey(chip8::Key::Eight, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::D))
        emulatorPtr_->setKey(chip8::Key::Nine, true);
    else
        emulatorPtr_->setKey(chip8::Key::Nine, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::Z))
        emulatorPtr_->setKey(chip8::Key::F, true);
    else
        emulatorPtr_->setKey(chip8::Key::F, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::C))
        emulatorPtr_->setKey(chip8::Key::B, true);
    else
        emulatorPtr_->setKey(chip8::Key::B, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::Num4))
        emulatorPtr_->setKey(chip8::Key::C, true);
    else
        emulatorPtr_->setKey(chip8::Key::C, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::R))
        emulatorPtr_->setKey(chip8::Key::D, true);
    else
        emulatorPtr_->setKey(chip8::Key::D, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::F))
        emulatorPtr_->setKey(chip8::Key::E, true);
    else
        emulatorPtr_->setKey(chip8::Key::E, false);

    if (Keyboard::isKeyPressed(Keyboard::Key::V))
        emulatorPtr_->setKey(chip8::Key::F, true);
    else
        emulatorPtr_->setKey(chip8::Key::F, false);

    return;
}
void resolveEvents(sf::RenderWindow& wdw)
{
    sf::Event event;
    while (wdw.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            wdw.close();
            break;
        default:
            break;
        }
    }
}
