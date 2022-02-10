#include <SFML/Audio.hpp>
#include "Audio.hpp"
#include "../Memory/Memory.hpp"

Audio::Audio(Memory* _MemoryBus) : MemoryBus(_MemoryBus) {
	AS = new AudioStream();
	AS->init();
	//AS->play();
}

Audio::~Audio() {
}

void Audio::Clock() {
}
