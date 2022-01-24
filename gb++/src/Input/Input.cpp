#include "Input.hpp"

bool InputHandler::GetInput() {
    while (EmuWindow->windowEvent(*EventVar)) {
        if (EventVar->type == sf::Event::Closed) {
            return false;
        } else if (EventVar->type == sf::Event::KeyPressed) {
            switch (EventVar->key.code) {
            case sf::Keyboard::X:
                EmulatorCore->Joypad[0] = 1;
                break;
            case sf::Keyboard::Z:
                EmulatorCore->Joypad[1] = 1;
                break;
            case sf::Keyboard::RShift:
                EmulatorCore->Joypad[2] = 1;
                break;
            case sf::Keyboard::Enter:
                EmulatorCore->Joypad[3] = 1;
                break;
            case sf::Keyboard::Left:
                EmulatorCore->Joypad[4] = 1;
                break;
            case sf::Keyboard::Right:
                EmulatorCore->Joypad[5] = 1;
                break;
            case sf::Keyboard::Up:
                EmulatorCore->Joypad[6] = 1;
                break;
            case sf::Keyboard::Down:
                EmulatorCore->Joypad[7] = 1;
                break;
            }
        }
        else if (EventVar->type == sf::Event::KeyReleased) {
            switch (EventVar->key.code) {
            case sf::Keyboard::X:
                EmulatorCore->Joypad[0] = 0;
                break;
            case sf::Keyboard::Z:
                EmulatorCore->Joypad[1] = 0;
                break;
            case sf::Keyboard::RShift:
                EmulatorCore->Joypad[2] = 0;
                break;
            case sf::Keyboard::Enter:
                EmulatorCore->Joypad[3] = 0;
                break;
            case sf::Keyboard::Left:
                EmulatorCore->Joypad[4] = 0;
                break;
            case sf::Keyboard::Right:
                EmulatorCore->Joypad[5] = 0;
                break;
            case sf::Keyboard::Up:
                EmulatorCore->Joypad[6] = 0;
                break;
            case sf::Keyboard::Down:
                EmulatorCore->Joypad[7] = 0;
                break;
            }
        }
    }

    return true;
}