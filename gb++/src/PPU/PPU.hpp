#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>

class Memory;

class PPU {	
	uint8_t BGPalette[4];
	uint8_t SpritePalette[2][4];

	enum PPUSTATE {
		HBLANK,
		VBLANK,
		OAM,
		DRAW
	};

	const uint8_t Colors[12] = {
		0xFF, 0xFF, 0xFF,
		0xAA, 0xAA, 0xAA,
		0x55, 0x55, 0x55,
		0x00, 0x00, 0x00
	};

	/*const uint8_t Colors[12] = {
		0x9F, 0xF4, 0xE5,
		0x00, 0xB9, 0xBE,
		0x00, 0x5F, 0x8C,
		0x00, 0x2B, 0x59
	};*/

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
	uint8_t WindowLine = 0;

	uint16_t SpriteLine;
	uint8_t SpriteXPos;
	uint8_t SpriteYPos;
	uint8_t SpriteFlags;
	uint8_t SpriteHeight;

	uint8_t OffsetX;
	uint8_t OffsetY;

	uint8_t STAT;
	uint16_t PPUCycles;

	PPUSTATE PastState;
	PPUSTATE State;

	bool STATEnabled;

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
	void UpdateSpritePalette(uint8_t paletteNo, uint8_t newPalette);
	void DrawFrameBuffer();
	void DrawBackground();
	void DrawWindow();
	void DrawSprites();
};

#endif