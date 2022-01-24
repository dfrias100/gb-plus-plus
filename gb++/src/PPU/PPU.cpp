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

	// TODO: Abstract these memory reads/writes
	switch (State) {
		case OAM:
			Byte = (MemoryBus->ReadWord(0xFF41) & 0xFC) | 0x02;
			MemoryBus->WriteWord(0xFF41, Byte);
			if ((Byte & 0x20) == 0x20 && PastState != OAM) {
				RaiseSTATInterrupt();
			}
			PastState = OAM;
			break;

		case DRAW:
			Byte = (MemoryBus->ReadWord(0xFF41) & 0xFC) | 0x03;
			MemoryBus->WriteWord(0xFF41, Byte);
			break;

		case HBLANK:
			Byte = (MemoryBus->ReadWord(0xFF41) & 0xFC) | 0x00;
			MemoryBus->WriteWord(0xFF41, Byte);
			if ((Byte & 0x08) == 0x08 && PastState != HBLANK) {
				RaiseSTATInterrupt();
			}
			PastState = HBLANK;
			break;

		case VBLANK:
			Byte = (MemoryBus->ReadWord(0xFF41) & 0xFC) | 0x01;
			MemoryBus->WriteWord(0xFF41, Byte);
			if ((Byte & 0x10) == 0x10 && PastState != VBLANK) {
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

	Byte = MemoryBus->ReadWord(0xFF44);
	if (PPUCycles >= 252 && Byte < 144 && UpdatingLine == false) {
		DrawBackground();
		UpdatingLine = true;
	}

	if (PPUCycles == 456) {
		PPUCycles = 0;
		Byte = MemoryBus->ReadWord(0xFF44) + 1;
		MemoryBus->WriteWord(0xFF44, Byte);
		UpdatingLine = false;
	}

	LC = MemoryBus->ReadWord(0xFF44);
	LCY = MemoryBus->ReadWord(0xFF45);
	Byte = MemoryBus->ReadWord(0xFF41);
	if (LC == LCY) {
		MemoryBus->WriteWord(0xFF41, Byte | 0x04);
		if (STATEnabled) {
			RaiseSTATInterrupt();
		}
	} else {
		MemoryBus->WriteWord(0xFF41, Byte & ~0x04);
	}

	if (MemoryBus->ReadWord(0xFF44) > 153) {
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
		}
	}
} 

void PPU::DrawBackground() {
	uint8_t Row = MemoryBus->ReadWord(0xFF44);
	uint8_t SCX = MemoryBus->ReadWord(0xFF43);
	uint8_t SCY = MemoryBus->ReadWord(0xFF42);

	uint16_t TileMapAddress = (MemoryBus->ReadWord(0xFF40) & 0x08) ? 0x9C00 : 0x9800;
	uint16_t TileDataBasePointer = (MemoryBus->ReadWord(0xFF40) & 0x10) ? 0x8000 : 0x9000;
	bool SignedAddressingMode = (MemoryBus->ReadWord(0xFF40) & 0x10) ? false : true;
	uint8_t PixelColor = 0;
	for (int i = 0; i < 256; i++) {
		uint8_t OffsetX = i + SCX;
		uint8_t OffsetY = Row + SCY;

		uint8_t TilePointer = MemoryBus->ReadWord(TileMapAddress + (OffsetX / 8) + 32 * (OffsetY / 8));

		uint8_t PixelBitL;
		uint8_t PixelBitH;
		uint16_t TileLine;
		if (SignedAddressingMode) {
			TileLine = TileDataBasePointer + ((int8_t)TilePointer * 0x10) + (OffsetY % 8 * 0x02);
			PixelBitL = (MemoryBus->ReadWord(TileLine) >> (7 - (OffsetX % 8))) & 0x01;
			PixelBitH = ((MemoryBus->ReadWord(TileLine + 1)) >> (7 - (OffsetX % 8))) & 0x01;
			PixelColor = PixelBitL | PixelBitH << 1;
 		} else {
			TileLine = TileDataBasePointer + (TilePointer * 0x10) + (OffsetY % 8 * 0x02);
			PixelBitL = (MemoryBus->ReadWord(TileLine) >> (7 - (OffsetX % 8))) & 0x01;
			PixelBitH = ((MemoryBus->ReadWord(TileLine + 1)) >> (7 - (OffsetX % 8))) & 0x01;
			PixelColor = PixelBitL | PixelBitH << 1;
		}

		uint8_t RealColor = BGPalette[PixelColor];
		BGLayer[Row * 256 * 4 + (i * 4)] = Colors[RealColor];
		BGLayer[Row * 256 * 4 + (i * 4) + 1] = Colors[RealColor];
		BGLayer[Row * 256 * 4 + (i * 4) + 2] = Colors[RealColor];
		BGLayer[Row * 256 * 4 + (i * 4) + 3] = 0xFF;
	}
}
