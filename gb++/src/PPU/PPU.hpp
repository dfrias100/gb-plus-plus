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
	
	uint8_t BGPalette[4];

	const uint8_t Colors[4] = {
		0xFF,
		0xAA,
		0x55,
		0x00
	};

	Memory* MemoryBus;
	uint8_t* Screen;
	uint8_t BGLayer[256 * 256 * 4];
	uint8_t WindowLayer[256 * 256 * 4];
	uint8_t ObjectLayer[256 * 256 * 4];
	uint8_t Byte;
	uint8_t LC;
	uint8_t LCY;
	bool STATEnabled;

	uint16_t PPUCycles;

	PPUSTATE PastState;
	PPUSTATE State;
public:
	bool FrameReady;
	bool FrameDrawn;
	bool UpdatingLine;

	PPU(Memory* _MemoryBus);
	~PPU();

	uint8_t* GetFrameBufferPointer();

	void Clock();
	void RaiseSTATInterrupt();
	void SetState();
	void UpdateBGPalette();
	void DrawFrameBuffer();
	void DrawBackground();
};

#endif