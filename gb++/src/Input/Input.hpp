#ifndef INPUT_HPP
#define INPUT_HPP

#include "../Window/Window.hpp"
#include "../Memory/Memory.hpp"

struct InputHandler {
	Window* EmuWindow;
	Memory* EmulatorCore;
	sf::Event* EventVar;

	bool GetInput();
};

#endif // !INPUT_HPP

