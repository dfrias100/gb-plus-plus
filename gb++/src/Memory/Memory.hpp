#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>

#include "../Sharp/Sharp.hpp"

class Memory {
	uint8_t* HighRAM;    // High RAM located at	0xFF80: 127 bytes
	uint8_t* IO;         // IO register located at 0xFF00: 76 bytes
	uint8_t* SpriteOAM;  // Video RAM located at 0xFE00: 160 bytes
	uint8_t* WorkingRAM; // Working RAM located at 0xC000 (mirrored at 0xE000): 8 kilobytes
	uint8_t* SwitchRAM;  // Switch RAM located at 0xA000: 8 kilobytes
	uint8_t* VideoRAM;   // Switch RAM located at 0x8000: 2 kilobytes
	uint8_t* Cartridge;  // Cartridge ROM beginning at 0x0000: 32 kilobytes

	uint8_t InterruptEnableRegister; // Interrupt Enable Register located at 0xFFFF: 1 byte

	Sharp* CPU; // The Memory class will need to tick the CPU itself

public:
	Memory();
	~Memory();

	void CPUWrite(uint16_t address, uint8_t data);
	uint8_t CPURead(uint16_t address);

	void CPUWrite16(uint16_t address, uint16_t data);
	uint16_t CPURead16(uint16_t address);

	void Clock();
};

#endif
