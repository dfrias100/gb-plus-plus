#include "Memory.hpp"
#include <iomanip>

Memory::Memory() {
	HighRAM	   = new uint8_t[127];
	IO         = new uint8_t[128];
	SpriteOAM  = new uint8_t[160];
	WorkingRAM = new uint8_t[1024 * 8];
	ExtRAM     = new uint8_t[1024 * 8];
	VideoRAM   = new uint8_t[1024 * 2];
	Cartridge  = new uint8_t[1024 * 32];

	InterruptEnableRegister = 0x00;

	std::fill(HighRAM, HighRAM + 127, 0x00);
	std::fill(SpriteOAM, SpriteOAM + 160, 0x00);
	std::fill(WorkingRAM, WorkingRAM + 1024 * 8, 0x00);
	std::fill(ExtRAM, ExtRAM + 1024 * 8, 0xFF);
	std::fill(VideoRAM, VideoRAM + 1024 * 2, 0x00);
	std::copy(IOPowerOn, IOPowerOn + 0x80, IO);

	InterruptFlags = &IO[0xF];

	SystemCycles = 0;
	SystemCyclesTMA = 0;

	CPU = new Sharp(this);
}

Memory::~Memory()
{
	delete[] HighRAM;
	delete[] IO;
	delete[] SpriteOAM;
	delete[] WorkingRAM;
	delete[] ExtRAM;
	delete[] VideoRAM;
	delete[] Cartridge;

	delete CPU;
}

void Memory::CPUWrite(uint16_t address, uint8_t data) {
	if (address <= 0x7FFF) {
		Cartridge[address] = data;
	} else if (address <= 0x9FFF) {
		VideoRAM[address - 0x8000] = data;
	} else if (address <= 0xBFFF) {
		ExtRAM[address - 0xA000] = data;
	} else if (address <= 0xDFFF) {
		WorkingRAM[address - 0xC000] = data;
	} else if (address <= 0xFDFF) {
		WorkingRAM[(address & 0xDFFF) - 0xC000] = data;
	} else if (address <= 0xFE9F) {
		SpriteOAM[address - 0xFE00] = data;
	} else if (address <= 0xFEFF) {
	} else if (address <= 0xFF7F) {
		IO[address - 0xFF00] = data;
		if (address == 0xFF02 && data == 0x81) {
			std::cout << CPURead(0xFF01);
		}
	} else if (address <= 0xFFFE) {
		HighRAM[address - 0xFF80] = data;
	} else if (address == 0xFFFF) {
		InterruptEnableRegister = data;
	}
}

uint8_t Memory::CPURead(uint16_t address) {
	if (address <= 0x7FFF) {
		return Cartridge[address];
	} else if (address <= 0x9FFF) {
		return VideoRAM[address - 0x8000];
	} else if (address <= 0xBFFF) {
		return ExtRAM[address - 0xA000];
	} else if (address <= 0xDFFF) {
		return WorkingRAM[address - 0xC000];
	} else if (address <= 0xFDFF) {
		return WorkingRAM[(address & 0xDFFF) - 0xC000];
	} else if (address <= 0xFE9F) {
		return SpriteOAM[address - 0xFE00];
	} else if (address <= 0xFEFF) {
		return 0x00;
	} else if (address <= 0xFF7F) {
		return IO[address - 0xFF00];
	} else if (address <= 0xFFFE) {
		return HighRAM[address - 0xFF80];
	} else if (address == 0xFFFF) {
		return InterruptEnableRegister;
	}
 }

void Memory::CPUWrite16(uint16_t address, uint16_t data) {
	CPUWrite(address, (uint8_t)(data & 0x00FF));
	CPUWrite(address + 1, (uint8_t)((data & 0xFF00) >> 8));
}

uint16_t Memory::CPURead16(uint16_t address) {
	return ((uint16_t) CPURead(address + 1) << 8) | CPURead(address);
}

void Memory::UpdateTimer() {
	// IO[0x04] is DIV
	// IO[0x05] is TIMA
	// IO[0x06] is TMA
	// IO[0x07] is TAC
	// IO[0x0F] is IF

	SystemCycles++;

	if (SystemCycles % 256 == 0) {
		IO[0x04]++;
	}

	if (IO[0x07] & 0x04) {
		SystemCyclesTMA++;
		while (SystemCyclesTMA >= TimerControl[IO[0x07] & 0x03]) {
			SystemCyclesTMA -= TimerControl[IO[0x07] & 0x03];
			IO[0x05]++;

			if (IO[0x05] == 0) {
				IO[0x0F] |= 0x04;
				IO[0x05] = IO[0x06];
			}
		}
	}
}

bool Memory::CartridgeLoader(std::string filename) {
	std::ifstream ROM;
	ROM.open(filename, std::ios::binary);
	ROM.read(reinterpret_cast<char *>(Cartridge), 1024 * 32);
	ROM.close();
	return true;
}


void Memory::Clock() {
	CPU->Clock();
	CPU->InterruptHandler();
	UpdateTimer();
}

