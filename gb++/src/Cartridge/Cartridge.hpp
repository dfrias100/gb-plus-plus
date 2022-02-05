#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "../Mapper/MBC0/MBC0.hpp"
#include "../Mapper/MBC1/MBC1.hpp"
#include "../Mapper/MBC2/MBC2.hpp"
#include "../Mapper/MBC3/MBC3.hpp"
#include "../Mapper/MBC5/MBC5.hpp"

class Cartridge {
	struct CartridgeHeader {
		char Title[16];
		uint8_t CGBFlag;
		uint8_t SGBFlag;
		uint8_t MapperType;
		uint8_t ROMSize;
		uint8_t RAMType;
	};

	uint8_t RTCClock[6] = {
		0, // Seconds
		0, // Minutes
		0, // Hours
		0, // Days - Low
		0x40, // Days - High
	};

	uint64_t ClockTicks = 0;

	std::vector<uint8_t> ROM;
	std::vector<uint8_t> ExtRAM;

	Mapper* CartridgeMapper;

public:
	struct CartridgeHeader Header;

	Cartridge(std::string FileName);
	~Cartridge();

	bool ReadWord(uint16_t address, uint8_t& data);
	bool WriteWord(uint16_t address, uint8_t data);
	void TickRTC();
};

#endif