#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>

class Memory;

class PPU {
	enum PPUSTATE {
		HBLANK,
		VBLANK,
		OAM,
		DRAW
	};

	Memory* MemoryBus;
	uint8_t* Screen;
	uint8_t Byte;
	uint8_t LC;
	uint8_t LCY;
	bool STATEnabled;

	uint16_t PPUCycles;

	PPUSTATE PastState;
	PPUSTATE State;
public:
	bool FrameReady;

	PPU(Memory* _MemoryBus);
	~PPU();

	void Clock();
	void RaiseSTATInterrupt();
	void SetState();
};

#endif