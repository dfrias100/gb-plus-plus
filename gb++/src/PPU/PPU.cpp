#if defined(__linux__)
    #include <algorithm>
#endif

#include "PPU.hpp"

#include "../Memory/Memory.hpp"

PPU::PPU(Memory* _MemoryBus) {
	MemoryBus = _MemoryBus;
	State = OAM;
	PastState = HBLANK;
	PPUCycles = 0;
	FrameReady = false;
	FrameDrawn = false;
	UpdatingLine = false;
	UpdateBGPalette();

	Screen = new uint8_t[160 * 144 * 4];
	std::fill(Screen, Screen + (160 * 144 * 4), 0xFF);
	PrevSpriteX.reserve(40);
}

PPU::~PPU() {
	delete[] Screen;
}

uint8_t* PPU::GetFrameBufferPointer() {
	return Screen;
}

void PPU::Clock() {
	SetState();

	STAT = MemoryBus->ReadWord(0xFF41);
	switch (State) {
		case OAM:
			STAT = (STAT & 0xFC) | 0x02;
			MemoryBus->WriteWord(0xFF41, STAT);
			if ((STAT & 0x20) == 0x20 && PastState != OAM) {
				RaiseSTATInterrupt();
			}
			PastState = OAM;
			break;

		case DRAW:
			STAT = (STAT & 0xFC) | 0x03;
			MemoryBus->WriteWord(0xFF41, STAT);
			break;

		case HBLANK:
			STAT = (STAT & 0xFC) | 0x00;
			MemoryBus->WriteWord(0xFF41, STAT);
			if ((STAT & 0x08) == 0x08 && PastState != HBLANK) {
				RaiseSTATInterrupt();
			}
			PastState = HBLANK;
			break;

		case VBLANK:
			STAT = (STAT & 0xFC) | 0x01;
			MemoryBus->WriteWord(0xFF41, STAT);
			if ((STAT & 0x10) == 0x10 && PastState != VBLANK) {
				RaiseSTATInterrupt();
			}
			PastState = VBLANK;

			if (!FrameDrawn && MemoryBus->ReadWord(0xFF44) == 144) {
				if (MemoryBus->ReadWord(0xFF40) >> 7) {
					Byte = MemoryBus->ReadWord(0xFF0F);
					MemoryBus->WriteWord(0xFF0F, Byte | 0x01);
					DrawFrameBuffer();
					FrameReady = true;
					std::fill(&BGLayer[0][0][0],& BGLayer[0][0][0] + 256 * 256 * 4, 0x00);
					std::fill(&WindowLayer[0][0][0], &WindowLayer[0][0][0] + 256 * 256 * 4, 0x00);
					std::fill(&ObjectLayer[0][0][0], &ObjectLayer[0][0][0] + 256 * 256 * 4, 0x00);
					WindowLine = 0;
				}
			}
			break;
	}

	PPUCycles++;

	LY = MemoryBus->ReadWord(0xFF44);
	if (PPUCycles >= 252 && LY < 144 && UpdatingLine == false) {
		DrawBackground();
		DrawWindow();
		DrawSprites();
		UpdatingLine = true;
	}

	if (PPUCycles == 456) {
		PPUCycles = 0;
		LY = MemoryBus->ReadWord(0xFF44) + 1;
		MemoryBus->WriteWord(0xFF44, LY);
		UpdatingLine = false;
	}

	LYC = MemoryBus->ReadWord(0xFF45);
	if (LY == LYC) {
		if ((STAT & 0x44) == 0x40) {
			MemoryBus->WriteWord(0xFF41, STAT | 0x04);
			RaiseSTATInterrupt();
		}
	} else {
		MemoryBus->WriteWord(0xFF41, STAT & ~0x04);
	}

	if (LY > 153) {
		MemoryBus->WriteWord(0xFF44, 0);
		FrameDrawn = false;
	}
}

void PPU::RaiseSTATInterrupt() {
	MemoryBus->WriteWord(0xFF0F, MemoryBus->ReadWord(0xFF0F) | 0x02);
}

void PPU::SetState() {
	if (PPUCycles < 80) {
		State = OAM;
	} else if (PPUCycles < 252) {
		State = DRAW;
	} else if (PPUCycles < 456) {
		State = HBLANK;
	}

	if (MemoryBus->ReadWord(0xFF44) >= 144) {
		State = VBLANK;
	}
}

void PPU::UpdateBGPalette() {
	Byte = MemoryBus->ReadWord(0xFF47);
	BGPalette[0] = (Byte & 0x03);
	BGPalette[1] = (Byte & 0x0C) >> 2;
	BGPalette[2] = (Byte & 0x30) >> 4;
	BGPalette[3] = (Byte & 0xC0) >> 6;
}

void PPU::UpdateSpritePalette(uint8_t paletteNo, uint8_t newPalette) {
	SpritePalette[paletteNo][0] = (newPalette & 0x03);
	SpritePalette[paletteNo][1] = (newPalette & 0x0C) >> 2;
	SpritePalette[paletteNo][2] = (newPalette & 0x30) >> 4;
	SpritePalette[paletteNo][3] = (newPalette & 0xC0) >> 6;
}

void PPU::DrawFrameBuffer() {
	for (int i = 0; i < 144; i++) {
		for (int j = 0; j < 160; j++) {
			Screen[(i * 160 * 4) + (j * 4)] = BGLayer[i][j][0];
			Screen[(i * 160 * 4) + (j * 4) + 1] = BGLayer[i][j][1];
			Screen[(i * 160 * 4) + (j * 4) + 2] = BGLayer[i][j][2];
			Screen[(i * 160 * 4) + (j * 4) + 3] = BGLayer[i][j][3];

			if (WindowLayer[i][j][3] != 0x00) {
				Screen[(i * 160 * 4) + (j * 4)] = WindowLayer[i][j][0];
				Screen[(i * 160 * 4) + (j * 4) + 1] = WindowLayer[i][j][1];
				Screen[(i * 160 * 4) + (j * 4) + 2] = WindowLayer[i][j][2];
				Screen[(i * 160 * 4) + (j * 4) + 3] = WindowLayer[i][j][3];
			}

			if (ObjectLayer[i][j][3] != 0x00) {
				Screen[(i * 160 * 4) + (j * 4)] = ObjectLayer[i][j][0];
				Screen[(i * 160 * 4) + (j * 4) + 1] = ObjectLayer[i][j][1];
				Screen[(i * 160 * 4) + (j * 4) + 2] = ObjectLayer[i][j][2];
				Screen[(i * 160 * 4) + (j * 4) + 3] = ObjectLayer[i][j][3];
			}
		}
	}
} 

void PPU::DrawBackground() {
	LY = MemoryBus->ReadWord(0xFF44);
	SCX = MemoryBus->ReadWord(0xFF43);
	SCY = MemoryBus->ReadWord(0xFF42);

	LCDC = MemoryBus->ReadWord(0xFF40);

	TileMapAddress = (LCDC & 0x08) ? 0x9C00 : 0x9800;
	TileDataBasePointer = (LCDC & 0x10) ? 0x8000 : 0x9000;
	bool SignedAddressingMode = (LCDC & 0x10) ? false : true;

	uint8_t PixelBitL;
	uint8_t PixelBitH;
	if (LCDC & 0x01) {
		for (int i = 0; i < 256; i++) {
			OffsetX = i + SCX;
			OffsetY = LY + SCY;

			TileID = MemoryBus->ReadWord(TileMapAddress + (OffsetX / 8) + 32 * (OffsetY / 8));

			TileLine = TileDataBasePointer + (OffsetY % 8 * 0x02);
			TileLine += SignedAddressingMode ? ((int8_t) TileID * 0x10) : TileID * 0x010;

			PixelBitL = (MemoryBus->ReadWord(TileLine) >> (7 - (OffsetX % 8))) & 0x01;
			PixelBitH = ((MemoryBus->ReadWord(TileLine + 1)) >> (7 - (OffsetX % 8))) & 0x01;
			PixelColor = PixelBitL | PixelBitH << 1;

			BGColorIndex[LY][i] = BGPalette[PixelColor];
			RealColor = BGPalette[PixelColor] * 3;
			BGLayer[LY][i][0] = Colors[RealColor];
			BGLayer[LY][i][1] = Colors[RealColor + 1];
			BGLayer[LY][i][2] = Colors[RealColor + 2];
			BGLayer[LY][i][3] = 0xFF;
		}
	}
}

void PPU::DrawWindow() {
	LY = MemoryBus->ReadWord(0xFF44);
	WY = MemoryBus->ReadWord(0xFF4A);
	WX = MemoryBus->ReadWord(0xFF4B);

	LCDC = MemoryBus->ReadWord(0xFF40);

	TileMapAddress = (LCDC & 0x40) ? 0x9C00 : 0x9800;
	TileDataBasePointer = (LCDC & 0x10) ? 0x8000 : 0x9000;
	bool SignedAddressingMode = (LCDC & 0x10) ? false : true;
	bool WindowLineDrawn = false;

	int16_t SignedWX = WX - 7;

	uint8_t PixelBitL;
	uint8_t PixelBitH;
	if ((LCDC & 0x01) && (LCDC & 0x20)) {
		for (int i = 0; i < 256; i++) {
			if ((SignedWX <= i && i < (SignedWX + 160)) && WindowLine < 144 && (WY <= LY && LY < WY + 144)) {
				WindowLineDrawn = i < 160 ? true : WindowLineDrawn;

				OffsetX = i - SignedWX;
				OffsetY = (WindowLine / 8) % 32;
				TileID = MemoryBus->ReadWord(TileMapAddress + (OffsetX / 8) + 32 * OffsetY);

				TileLine = TileDataBasePointer + ((WindowLine % 8) * 0x02);
				TileLine += SignedAddressingMode ? ((int8_t) TileID * 0x10) : TileID * 0x010;

				PixelBitL = (MemoryBus->ReadWord(TileLine) >> (7 - (OffsetX % 8))) & 0x01;
				PixelBitH = ((MemoryBus->ReadWord(TileLine + 1)) >> (7 - (OffsetX % 8))) & 0x01;
				PixelColor = PixelBitL | PixelBitH << 1;

				WindowColorIndex[LY][i] = BGPalette[PixelColor];
				RealColor = BGPalette[PixelColor] * 3;
				WindowLayer[LY][i][0] = Colors[RealColor];
				WindowLayer[LY][i][1] = Colors[RealColor + 1];
				WindowLayer[LY][i][2] = Colors[RealColor + 2];
				WindowLayer[LY][i][3] = 0xFF;
			}
		}

		if (WindowLineDrawn)
			WindowLine++;
	}
}

void PPU::DrawSprites() {
	LY = MemoryBus->ReadWord(0xFF44);
	LCDC = MemoryBus->ReadWord(0xFF40);
	PixelColor = 0x00;

	uint16_t PatternTable = 0x8000;

	uint8_t PixelBitL;
	uint8_t PixelBitH;

	uint8_t HeightOffset;
	uint8_t XShift;

	if (LCDC & 0x02) {
		for (uint16_t OAMAddr = 0xFE00; OAMAddr < 0xFEA0; OAMAddr += 4) {
			SpriteYPos = MemoryBus->ReadWord(OAMAddr);
			SpriteXPos = MemoryBus->ReadWord(OAMAddr + 1);
			TileID = MemoryBus->ReadWord(OAMAddr + 2);
			SpriteFlags = MemoryBus->ReadWord(OAMAddr + 3);
			SpriteHeight = (LCDC >> 2) & 0x1 ? 16 : 8;
			uint8_t FlipCase = (SpriteFlags & 0x60) >> 5;
			uint8_t SameSpriteXCount = std::count(PrevSpriteX.begin(), PrevSpriteX.end(), SpriteXPos);

			TileID &= SpriteHeight == 16 ? 0xFE : 0xFF;

			if (LY >= (SpriteYPos - 16) && LY < ((SpriteYPos - 16) + SpriteHeight)) {
				for (int i = 0; i < 8; i++) {
					HeightOffset = FlipCase & 0x02 ? (SpriteHeight - 1) - (LY - (SpriteYPos - 16)) : (LY - (SpriteYPos - 16));
					XShift = FlipCase & 0x01 ? i : 7 - i;

					SpriteLine = PatternTable + TileID * 0x10 + HeightOffset * 0x02;
					PixelBitL = (MemoryBus->ReadWord(SpriteLine) >> XShift) & 0x01;
					PixelBitH = (MemoryBus->ReadWord(SpriteLine + 1) >> XShift) & 0x01;
					PixelColor = PixelBitL | PixelBitH << 1;

					uint16_t PaletteNum = (SpriteFlags >> 4) & 0x01;
					
					RealColor = SpritePalette[PaletteNum][PixelColor] * 3;

					bool BGColorIsNotZero = BGColorIndex[LY][(SpriteXPos + i - 8)] != 0;
					bool WindowColorIsNotZero = WindowColorIndex[LY][(SpriteXPos + i - 8)] != 0;
					bool OpaqueWithSameX = ObjectLayer[LY][(SpriteXPos + i - 8)][3] == 0xFF && SameSpriteXCount;

					if (((SpriteFlags & 0x80) && (BGColorIsNotZero || WindowColorIsNotZero)) || OpaqueWithSameX) {
						continue;
					}

					if (PixelColor != 0 && (SpriteXPos + i) <= 0xFF) {
						ObjectLayer[LY][(SpriteXPos + i - 8)][0] = Colors[RealColor];
						ObjectLayer[LY][(SpriteXPos + i - 8)][1] = Colors[RealColor + 1];
						ObjectLayer[LY][(SpriteXPos + i - 8)][2] = Colors[RealColor + 2];
						ObjectLayer[LY][(SpriteXPos + i - 8)][3] = 0xFF;
					}
				}
			}
			PrevSpriteX.push_back(SpriteXPos);
		}
	}

	PrevSpriteX.clear();
}
