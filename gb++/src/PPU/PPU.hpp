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

	uint8_t LY;
	uint8_t LYC;

	uint8_t SCX;
	uint8_t SCY;

	uint8_t WX;
	uint8_t WY;

	uint8_t LCDC;

	uint8_t PixelColor;
	uint8_t RealColor;

	uint8_t TileID;
	uint16_t TileMapAddress;
	uint16_t TileDataBasePointer;
	uint16_t TileLine;

	uint16_t SpriteLine;
	uint8_t SpriteXPos;
	uint8_t SpriteYPos;
	uint8_t SpriteFlags;
	uint8_t SpriteHeight;

	uint8_t OffsetX;
	uint8_t OffsetY;

	uint8_t STAT;

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
	void DrawWindow();
	void DrawSprites();
};

#endif