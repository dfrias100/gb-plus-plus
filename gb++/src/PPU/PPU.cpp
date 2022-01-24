#include "PPU.hpp"

#include "../Memory/Memory.hpp"

PPU::PPU(Memory* _MemoryBus) {
	MemoryBus = _MemoryBus;
	State = OAM;
	PPUCycles = 0;
	FrameReady = false;
	FrameDrawn = false;
	UpdatingLine = false;
	UpdateBGPalette();

	Screen = new uint8_t[160 * 144 * 4];
	std::fill(Screen, Screen + (160 * 144 * 4), 0xFF);
}

PPU::~PPU() {
	delete[] Screen;
}

uint8_t* PPU::GetFrameBufferPointer() {
	return Screen;
}

void PPU::Clock() {
	SetState();
	STATEnabled = MemoryBus->InterruptEnableRegister & 0x02;

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
					std::fill(BGLayer, BGLayer + 256 * 256 * 4, 0x00);
					std::fill(WindowLayer, WindowLayer + 256 * 256 * 4, 0x00);
					std::fill(ObjectLayer, ObjectLayer + 256 * 256 * 4, 0x00);
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
		MemoryBus->WriteWord(0xFF41, STAT | 0x04);
		if (STATEnabled && (STAT & 0x40)) {
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

void PPU::DrawFrameBuffer() {
	for (int i = 0; i < 144; i++) {
		for (int j = 0; j < 160; j++) {
			Screen[(i * 160 * 4) + (j * 4)] = BGLayer[(i * 256 * 4) + (j * 4)];
			Screen[(i * 160 * 4) + (j * 4) + 1] = BGLayer[(i * 256 * 4) + (j * 4) + 1];
			Screen[(i * 160 * 4) + (j * 4) + 2] = BGLayer[(i * 256 * 4) + (j * 4) + 2];
			Screen[(i * 160 * 4) + (j * 4) + 3] = BGLayer[(i * 256 * 4) + (j * 4) + 3];

			if (WindowLayer[(i * 256 * 4) + (j * 4) + 3] != 0x00) {
				Screen[(i * 160 * 4) + (j * 4)] = WindowLayer[(i * 256 * 4) + (j * 4)];
				Screen[(i * 160 * 4) + (j * 4) + 1] = WindowLayer[(i * 256 * 4) + (j * 4) + 1];
				Screen[(i * 160 * 4) + (j * 4) + 2] = WindowLayer[(i * 256 * 4) + (j * 4) + 2];
				Screen[(i * 160 * 4) + (j * 4) + 3] = WindowLayer[(i * 256 * 4) + (j * 4) + 3];
			}

			if (ObjectLayer[(i * 256 * 4) + (j * 4) + 3] != 0x00) {
				Screen[(i * 160 * 4) + (j * 4)] = ObjectLayer[(i * 256 * 4) + (j * 4)];
				Screen[(i * 160 * 4) + (j * 4) + 1] = ObjectLayer[(i * 256 * 4) + (j * 4) + 1];
				Screen[(i * 160 * 4) + (j * 4) + 2] = ObjectLayer[(i * 256 * 4) + (j * 4) + 2];
				Screen[(i * 160 * 4) + (j * 4) + 3] = ObjectLayer[(i * 256 * 4) + (j * 4) + 3];
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
	for (int i = 0; i < 256; i++) {
		OffsetX = i + SCX;
		OffsetY = LY + SCY;

		TileID = MemoryBus->ReadWord(TileMapAddress + (OffsetX / 8) + 32 * (OffsetY / 8));

		TileLine = TileDataBasePointer + (OffsetY % 8 * 0x02);
		TileLine += SignedAddressingMode ? ((int8_t) TileID * 0x10) : TileID * 0x010;

		PixelBitL = (MemoryBus->ReadWord(TileLine) >> (7 - (OffsetX % 8))) & 0x01;
		PixelBitH = ((MemoryBus->ReadWord(TileLine + 1)) >> (7 - (OffsetX % 8))) & 0x01;
		PixelColor = PixelBitL | PixelBitH << 1;

		uint8_t RealColor = BGPalette[PixelColor];
		BGLayer[LY * 256 * 4 + (i * 4)] = Colors[RealColor];
		BGLayer[LY * 256 * 4 + (i * 4) + 1] = Colors[RealColor];
		BGLayer[LY * 256 * 4 + (i * 4) + 2] = Colors[RealColor];
		BGLayer[LY * 256 * 4 + (i * 4) + 3] = 0xFF;
	}
}

void PPU::DrawWindow() {
	LY = MemoryBus->ReadWord(0xFF44);
	WY = MemoryBus->ReadWord(0xFF4A);
	WX = MemoryBus->ReadWord(0xFF4B) - 7;

	LCDC = MemoryBus->ReadWord(0xFF40);

	TileMapAddress = (LCDC & 0x20) ? 0x9C00 : 0x9800;
	TileDataBasePointer = (LCDC & 0x10) ? 0x8000 : 0x9000;
	bool SignedAddressingMode = (LCDC & 0x10) ? false : true;

	uint8_t PixelBitL;
	uint8_t PixelBitH;
	if (LCDC & 0x20) {
		for (int i = 0; i < 256; i++) {
			if ((WX <= i && i < WX + 160) && (WY <= LY && LY < WY + 144)) {
				OffsetX = i - WX;
				OffsetY = LY - WY;
				TileID = MemoryBus->ReadWord(TileMapAddress + (OffsetX / 8) + 32 * (OffsetY / 8));

				TileLine = TileDataBasePointer + (OffsetY % 8 * 0x02);
				TileLine += SignedAddressingMode ? ((int8_t) TileID * 0x10) : TileID * 0x010;

				PixelBitL = (MemoryBus->ReadWord(TileLine) >> (7 - (OffsetX % 8))) & 0x01;
				PixelBitH = ((MemoryBus->ReadWord(TileLine + 1)) >> (7 - (OffsetX % 8))) & 0x01;
				PixelColor = PixelBitL | PixelBitH << 1;

				RealColor = BGPalette[PixelColor];
				WindowLayer[LY * 256 * 4 + (i * 4)] = Colors[RealColor];
				WindowLayer[LY * 256 * 4 + (i * 4) + 1] = Colors[RealColor];
				WindowLayer[LY * 256 * 4 + (i * 4) + 2] = Colors[RealColor];
				WindowLayer[LY * 256 * 4 + (i * 4) + 3] = 0xFF;
			}
		}
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

			if (LY >= (SpriteYPos - 16) && LY < ((SpriteYPos - 16) + SpriteHeight)) {
				for (int i = 0; i < SpriteHeight; i++) {
					for (int j = 0; j < 8; j++) {
						HeightOffset = FlipCase & 0x02 ? (SpriteHeight - 1) - i : i;
						XShift = FlipCase & 0x01 ? j : 7 - j;

						SpriteLine = PatternTable + TileID * 0x10 + HeightOffset * 0x02;
						PixelBitL = (MemoryBus->ReadWord(SpriteLine) >> XShift) & 0x01;
						PixelBitH = (MemoryBus->ReadWord(SpriteLine + 1) >> XShift) & 0x01;
						PixelColor = PixelBitL | PixelBitH << 1;


						// TODO: Implement Sprite palette
						uint16_t SpritePalette = (SpriteFlags >> 4) & 0x01 ? 0xFF49 : 0xFF48;
						RealColor = (MemoryBus->ReadWord(SpritePalette) >> (2 * PixelColor)) & 0x03;
						
						if (PixelColor != 0 && (SpriteYPos + i) <= 0xFF && (SpriteXPos + j) <= 0xFF) {
							ObjectLayer[(SpriteYPos + i - 16) * 256 * 4 + ((SpriteXPos + j - 8) * 4)] = Colors[RealColor];
							ObjectLayer[(SpriteYPos + i - 16) * 256 * 4 + ((SpriteXPos + j - 8) * 4) + 1] = Colors[RealColor];
							ObjectLayer[(SpriteYPos + i - 16) * 256 * 4 + ((SpriteXPos + j - 8) * 4) + 2] = Colors[RealColor];
							ObjectLayer[(SpriteYPos + i - 16) * 256 * 4 + ((SpriteXPos + j - 8) * 4) + 3] = 0xFF;
						}
					}
				}
			}
		}
	}
}
