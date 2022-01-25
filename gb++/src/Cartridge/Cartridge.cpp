#include "Cartridge.hpp"

Cartridge::Cartridge(std::string FileName) {
	std::ifstream File;
	File.open(FileName, std::ios::binary);

	if (File.is_open()) {
		File.seekg(0x134);

		char TempBuf[16];
		File.read(TempBuf, 16);
		strncpy_s(Header.Title, TempBuf, 16);
		Header.CGBFlag = TempBuf[15];

		File.seekg(0x146);
		File.read(reinterpret_cast<char *>(&Header) + 17, 4);

		uint32_t TrueROMSize = 0x20 << Header.ROMSize;
		uint8_t RAMBanks = 0;

		std::cout << "ROM title: "   << Header.Title             << std::endl;
		std::cout << "CGB flag: "    << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.CGBFlag    << std::endl;
		std::cout << "SGB flag: "    << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.SGBFlag    << std::endl;
		std::cout << "Mapper type: " << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.MapperType << std::endl;
		std::cout << "ROM size: "    << std::dec << TrueROMSize << " bytes " << std::endl;
		std::cout << "RAM type: "    << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.RAMType    << std::endl;

		if (Header.CGBFlag == 0xC0) {
			std::cout << "ROM not supported! This works in CGB mode only which is not emulated." << std::endl;
			exit(0);
		}
		
		if (Header.SGBFlag == 0x03) {
			std::cout << "The SGB flag is set in this cartridge, however it will NOT run in SGB mode." << std::endl;
		}

		ROM.resize(TrueROMSize * 1024);

		switch (Header.RAMType) {
			case 0x00:
				ExtRAM.resize(0);
				RAMBanks = 0;
				break;
		}

		switch (Header.MapperType) {
			case 0x00:
				CartridgeMapper = new MBC0(1, RAMBanks);
				break;
		}
		
		File.read(reinterpret_cast<char *>(ROM.data()), TrueROMSize * 1024);
		File.close();
	}
}

Cartridge::~Cartridge() {
}

bool Cartridge::ReadWord(uint16_t address, uint8_t& data) {
	uint16_t NewAddress;
	if (address >= 0x0000 && address < 0x8000) {
		CartridgeMapper->ROMReadMappedWord(address, NewAddress);
		data = ROM[NewAddress];
		return true;
	} else if (address >= 0xA000 && address < 0xC000) {
		if (CartridgeMapper->RAMReadMappedWord(address, NewAddress)) {
			data = ExtRAM[NewAddress];
		}
		return true;
	}
	return false;
}

bool Cartridge::WriteWord(uint16_t address, uint8_t data) {
	if (address >= 0x0000 && address < 0x8000) {
		return true;
	} else if (address >= 0xA000 && address < 0xC000) {
		return true;
	}
	return false;
}
