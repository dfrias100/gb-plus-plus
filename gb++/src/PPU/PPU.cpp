#include "PPU.hpp"

#include "../Memory/Memory.hpp"

PPU::PPU(Memory* _MemoryBus) {
	MemoryBus = _MemoryBus;
	State = OAM;
	PPUCycles = 0;
}

PPU::~PPU() {
}

void PPU::Clock() {
	PPUCycles++;

	SetState();
	STATEnabled = MemoryBus->InterruptEnableRegister & 0x02;

	// TODO: Abstract these memory reads/writes
	switch (State) {
		case OAM:
			Byte = MemoryBus->ReadWord(0xFF41);
			MemoryBus->WriteWord(0xFF41, (Byte & 0xFC) | 0x02);
			if ((Byte & 0x20) == 0x20 && PastState != OAM) {
				RaiseSTATInterrupt();
			}
			PastState = OAM;
			break;

		case DRAW:
			Byte = MemoryBus->ReadWord(0xFF41);
			MemoryBus->WriteWord(0xFF41, (Byte & 0xFC) | 0x03);
			break;

		case HBLANK:
			Byte = MemoryBus->ReadWord(0xFF41);
			MemoryBus->WriteWord(0xFF41, (Byte & 0xFC) | 0x00);
			if ((Byte & 0x08) == 0x08 && PastState != HBLANK) {
				RaiseSTATInterrupt();
			}
			PastState = HBLANK;
			break;

		case VBLANK:
			Byte = MemoryBus->ReadWord(0xFF41);
			MemoryBus->WriteWord(0xFF41, (Byte & 0xFC) | 0x01);
			if ((Byte & 0x10) == 0x10 && PastState != VBLANK) {
				RaiseSTATInterrupt();
			}
			PastState = VBLANK;
			break;
	}

	if (MemoryBus->ReadWord(0xFF44) == 144) {
		if (MemoryBus->ReadWord(0xFF40) >> 7) {
			Byte = MemoryBus->ReadWord(0xFF0F);
			MemoryBus->WriteWord(0xFF0F, Byte | 0x01);
		}
	}

	if (PPUCycles > 456) {
		PPUCycles -= 456;
		Byte = MemoryBus->ReadWord(0xFF44) + 1;
		MemoryBus->WriteWord(0xFF44, Byte);
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

	if (MemoryBus->ReadWord(0xFF44) > 154) {
		MemoryBus->WriteWord(0xFF44, 0);
	}
}

void PPU::RaiseSTATInterrupt() {
	MemoryBus->WriteWord(0xFF0F, MemoryBus->ReadWord(0xFF0F) | 0x02);
}

void PPU::SetState() {
	if (PPUCycles < 81) {
		State = OAM;
	} else if (PPUCycles < 253) {
		State = DRAW;
	} else if (PPUCycles < 457) {
		State = HBLANK;
	}
	
	if (MemoryBus->ReadWord(0xFF44) >= 144) {
		State = VBLANK;
	}
}
