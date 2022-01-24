#include "Memory.hpp"
#include <iomanip>

Memory::Memory() {
	HighRAM	   = new uint8_t[127];
	IO         = new uint8_t[128];
	SpriteOAM  = new uint8_t[160];
	WorkingRAM = new uint8_t[1024 * 8];
	ExtRAM     = new uint8_t[1024 * 8];
	VideoRAM   = new uint8_t[1024 * 8];
	Cartridge  = new uint8_t[1024 * 32];
	BootROM	   = new uint8_t[256];

	InterruptEnableRegister = 0x00;

	std::fill(HighRAM, HighRAM + 127, 0x00);
	std::fill(SpriteOAM, SpriteOAM + 160, 0x00);
	std::fill(WorkingRAM, WorkingRAM + 1024 * 8, 0x00);
	std::fill(ExtRAM, ExtRAM + 1024 * 8, 0xFF);
	std::fill(VideoRAM, VideoRAM + 1024 * 8, 0x00);

	InterruptFlags = &IO[0xF];

	SystemCycles = 0;
	SystemCyclesTMA = 0;

	CPU = new Sharp(this);
	GPU = new PPU(this);

	if (BootROMEnable) {
		CPU->SetupBootRom();
		std::fill(IO, IO + 128, 0x00);
	} else {
		std::copy(IOPowerOn, IOPowerOn + 0x80, IO);
	}
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

void Memory::WriteWord(uint16_t address, uint8_t data) {
	if (address <= 0xFF && BootROMEnable) {
		BootROM[address] = data;
	} else if (address <= 0x7FFF) {
		//Cartridge[address] = data;
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
		if (address == 0xFF00) {
			IO[0x00] = UpdateJoypad(data);
		} else if (address == 0xFF46) {
			OAMDMACopy();
		} else if (address == 0xFF47) {
			GPU->UpdateBGPalette();
		} else if (address == 0xFF50) {
			BootROMEnable = false;
		} else if (address == 0xFF02 && data == 0x81) {
			std::cout << ReadWord(0xFF01);
		} else if (address == 0xFF0F) {
			IO[0x0F] = 0xE0 | (data & 0x1F);
		}
	} else if (address <= 0xFFFE) {
		HighRAM[address - 0xFF80] = data;
	} else if (address == 0xFFFF) {
		InterruptEnableRegister = data;
	}
}

uint8_t Memory::ReadWord(uint16_t address) {
	if (address <= 0xFF && BootROMEnable) {
		return BootROM[address];
	} else if (address <= 0x7FFF) {
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

void Memory::WriteDoubleWord(uint16_t address, uint16_t data) {
	WriteWord(address, (uint8_t)(data & 0x00FF));
	WriteWord(address + 1, (uint8_t)((data & 0xFF00) >> 8));
}

uint16_t Memory::ReadDoubleWord(uint16_t address) {
	return ((uint16_t) ReadWord(address + 1) << 8) | ReadWord(address);
}

void Memory::OAMDMACopy() {
	uint16_t source = IO[0x46] << 8;
	uint16_t end = source + 0xA0;

	for (; source < end; source++) {
		SpriteOAM[source & 0xFF] = ReadWord(source);
	}
}

uint8_t Memory::UpdateJoypad(uint8_t button_mask) {
	// This will hold which buttons are pressed
	uint8_t buttons = 0x00;

	// Check which set of buttons we will check
	// The emulator sets a pushed button to 1, but
	// on the gameboy it sets it to 0, so we invert the bit
	if ((button_mask & 0x30) == 0x10) {
		buttons |= ~Joypad[0] & 0x1;
		buttons |= (~Joypad[1] & 0x1) << 1;
		buttons |= (~Joypad[2] & 0x1) << 2;
		buttons |= (~Joypad[3] & 0x1) << 3;
	} else if ((button_mask & 0x30) == 0x20) {
		buttons |= ~Joypad[5] & 0x1;
		buttons |= (~Joypad[4] & 0x1) << 1;
		buttons |= (~Joypad[6] & 0x1) << 2;
		buttons |= (~Joypad[7] & 0x1) << 3;
	}

	// Clear the lower four bits and or them with the two unused bits
	button_mask = (button_mask & 0xF0) | 0xC0;

	// Return the entire register with the buttons that are NOT pressed
	return button_mask | buttons;
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

bool Memory::LoadBootRom() {
	std::ifstream BOOTROM;
	BOOTROM.open("bios/dmg_boot.bin", std::ios::binary);
	BOOTROM.read(reinterpret_cast<char*>(BootROM), 256);
	BOOTROM.close();
	return true;
}


void Memory::Clock() {
	CPU->Clock();
	GPU->Clock();
	CPU->InterruptHandler();
	UpdateTimer();
}