#include <iostream>
#include <cstdint>

#include "Window/Window.hpp"
#include "Memory/Memory.hpp" 

const unsigned int GB_SCREEN_WIDTH = 160;
const unsigned int GB_SCREEN_HEIGHT = 144;

int main(int argc, char* argv[]) {
    bool exit = false;
    uint8_t* frameBuffer = new uint8_t[GB_SCREEN_WIDTH * GB_SCREEN_HEIGHT * 4];
    std::fill(frameBuffer, frameBuffer + (GB_SCREEN_WIDTH * GB_SCREEN_HEIGHT * 4), 0xFF);

    Window emuWindow(GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT, frameBuffer);

    sf::Event emuEvent;

    /*while (!exit) {
        while (emuWindow.windowEvent(emuEvent)) {
            if (emuEvent.type == sf::Event::Closed) {
                exit = true;
            }
        }

        emuWindow.draw();
    }*/

    Z80 CPU;

    CPU.AF = 0x0F;

    std::cout << "AF register: " << CPU.AF << std::endl;
    std::cout << "High of the AF register: " << (int) CPU.A << std::endl;
    std::cout << "Low of the AF register: " << (int) CPU.F << std::endl;


    delete[] frameBuffer;

	return 0;
}