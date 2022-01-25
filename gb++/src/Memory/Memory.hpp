#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <string>

#include "../Sharp/Sharp.hpp"
#include "../Cartridge/Cartridge.hpp"
#include "../PPU/PPU.hpp"

class Memory {
	uint8_t* HighRAM;    // High RAM located at	0xFF80: 127 bytes
	uint8_t* IO;         // IO register located at 0xFF00: 128 bytes
	uint8_t* SpriteOAM;  // Video RAM located at 0xFE00: 160 bytes
	uint8_t* WorkingRAM; // Working RAM located at 0xC000 (mirrored at 0xE000): 8 kilobytes
	uint8_t* VideoRAM;   // Video RAM located at 0x8000: 8 kilobytes

	// These elements are in the cartridge class now
	// External RAM located at 0xA000: 8 kilobyte address space
	// Cartridge ROM beginning at 0x0000: 32 kilobyte address space

	uint8_t* BootROM;
	Cartridge* ConnectedCartridge;

	Sharp* CPU; // The Memory class will need to tick the CPU itself

	const uint8_t IOPowerOn[0x80]{
		0xCF, 0x00, 0x7E, 0xFF, 0xAB, 0x00, 0x00, 0xF8,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
		0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00,
		0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
		0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x0C, 0x7D, 0x80, 0x75, 0xC6, 0x7F, 0x9D, 0xEF,
		0x22, 0x3B, 0x22, 0x56, 0x49, 0xFE, 0x4A, 0xFE,
		0x91, 0x85, 0x00, 0x00, 0x90, 0x00, 0xFF, 0xFC,
		0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	const int TimerControl[4] {
		1024,
		  16,
		  64,
		 256
	};

public:
	PPU* GPU;

	uint8_t Joypad[8] = { 
		0, // A
		0, // B
		0, // SELECT
		0, // START
		0, // LEFT 
		0, // RIGHT
		0, // UP
		0, // DOWN
	};

	uint8_t InterruptEnableRegister; // Interrupt Enable Register located at 0xFFFF: 1 byte
	uint8_t* InterruptFlags;

	bool BootROMEnable = true;

	// Ticks
	int64_t SystemCycles;
	int64_t SystemCyclesTMA;

	Memory();
	~Memory();

	void WriteWord(uint16_t address, uint8_t data);
	uint8_t ReadWord(uint16_t address);

	void WriteDoubleWord(uint16_t address, uint16_t data);
	uint16_t ReadDoubleWord(uint16_t address);

	void OAMDMACopy();
	uint8_t UpdateJoypad(uint8_t button_mask);
	void UpdateTimer();

	void CartridgeLoader(Cartridge* Cart);
	bool LoadBootRom();

	void Clock();
};

#endif
