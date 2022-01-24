#include <iostream>
#include <cstdint>
#include <chrono>

#include "Window/Window.hpp"
#include "Memory/Memory.hpp" 
#include "Input/Input.hpp"

const unsigned int GB_SCREEN_WIDTH = 160;
const unsigned int GB_SCREEN_HEIGHT = 144;

int main(int argc, char* argv[]) {
    Memory GB;
    Window EmuWindow(GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT, GB.GPU->GetFrameBufferPointer());
    sf::Event EmuEvent;

    struct InputHandler Input;
    Input.EmuWindow = &EmuWindow;
    Input.EmulatorCore = &GB;
    Input.EventVar = &EmuEvent;

    if (argc > 1)
        GB.CartridgeLoader(argv[1]);

    GB.LoadBootRom();

    float ResidualTime = 0.0f;
    float ElapsedTime = 0.0f;
    sf::Clock Clock;
    float Start = 0.0f;

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    start = std::chrono::system_clock::now();
    while (Input.GetInput()) {
        if (ResidualTime > 0.0f) {
           ResidualTime -= ElapsedTime;
        } else {
            ResidualTime += (1.0f / 59.73f) - ElapsedTime;
            do {
                GB.Clock(); 
            } while (!GB.GPU->FrameReady);
            GB.GPU->FrameReady = false;
            GB.GPU->FrameDrawn = true;
        }
        
        EmuWindow.draw();

        ElapsedTime = Clock.getElapsedTime().asSeconds() - Start;
        Start = Clock.getElapsedTime().asSeconds();
    }
    end = std::chrono::system_clock::now();

    auto duration = end - start;

    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms, Cycles: " << GB.SystemCycles << std::endl;
    std::cout << "Average clock speed: " << ((float)GB.SystemCycles / std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) * (1000.0f / 1e6f) << " MHz" << std::endl;

	return 0;
}