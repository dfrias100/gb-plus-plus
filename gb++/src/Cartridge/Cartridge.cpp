#include "Cartridge.hpp"
#ifdef UNIX
    #include <cstring>
#endif

Cartridge::Cartridge(std::string FileName) {
	std::ifstream File;
	File.open(FileName, std::ios::binary);

	if (File.is_open()) {
		File.seekg(0x134);

		char TempBuf[16];
		File.read(TempBuf, 16);
		
		#ifdef WIN32
		strncpy_s(Header.Title, TempBuf, 16);
		#elif defined(UNIX)
		strncpy(Header.Title, TempBuf, 16);
		#endif
		
		Header.CGBFlag = TempBuf[15];

		File.seekg(0x146);
		File.read(reinterpret_cast<char *>(&Header) + 17, 4);

		uint64_t TrueROMSize = 0x20llu << Header.ROMSize;
		uint8_t RAMBanks = 0;
		uint16_t ROMBanks = 0;

		std::cout << "ROM title: "   << Header.Title             << std::endl;
		std::cout << "CGB flag: "    << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.CGBFlag    << std::endl;
		std::cout << "SGB flag: "    << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.SGBFlag    << std::endl;
		std::cout << "Mapper type: " << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.MapperType << std::endl;
		std::cout << "ROM size: "    << std::dec << TrueROMSize << " kilobytes " << std::endl;
		std::cout << "RAM type: "    << std::hex << std::setw(2) << std::setfill('0') << (uint16_t) Header.RAMType    << std::endl;

		if (Header.CGBFlag == 0xC0) {
			std::cout << "ROM not supported! This works in CGB mode only which is not emulated." << std::endl;
			exit(1);
		}
		
		if (Header.SGBFlag == 0x03) {
			std::cout << "The SGB flag is set in this cartridge, however it will NOT run in SGB mode." << std::endl;
		}

		switch (Header.RAMType) {
			case 0x00:
				ExtRAM.resize(0);
				RAMBanks = 0;
				break;
			case 0x02:
				ExtRAM.resize(1024 * 8);
				RAMBanks = 1;
				break;
			case 0x03:
				ExtRAM.resize(1024 * 32);
				RAMBanks = 4;
				break;
		}

		switch (Header.ROMSize) {
			case 0x00:
				ROMBanks = 2;
				break;
			case 0x01:
				ROMBanks = 4;
				break;
			case 0x02:
				ROMBanks = 8;
				break;
			case 0x03:
				ROMBanks = 16;
				break;
			case 0x04:
				ROMBanks = 32;
				break;
			case 0x05:
				ROMBanks = 64;
				break;
			case 0x06:
				ROMBanks = 128;
				break;
			case 0x07:
				ROMBanks = 256;
				break;
			case 0x08:
				ROMBanks = 512;
				break;
			default:
				std::cout << "ROM bank count not supported. Exiting." << std::endl;
				break;
		}

		ROM.resize(TrueROMSize * 1024llu);

		switch (Header.MapperType) {
			case 0x00:
				CartridgeMapper = new MBC0(ROMBanks, RAMBanks);
				break;
			case 0x01:
			case 0x02:
			case 0x03:
				CartridgeMapper = new MBC1(ROMBanks, RAMBanks);
				break;
			case 0x05:
			case 0x06:
				ExtRAM.resize(512);
				CartridgeMapper = new MBC2(ROMBanks, RAMBanks);
				break;
			case 0x12:
			case 0x13:
				CartridgeMapper = new MBC3(ROMBanks, RAMBanks);
				break;
			case 0x0F:
			case 0x10:
				RTCClock[4] = 0x00;
				CartridgeMapper = new MBC3(ROMBanks, RAMBanks);
				break;
			case 0x19:
			case 0x1A:
			case 0x1B:
				CartridgeMapper = new MBC5(ROMBanks, RAMBanks);
				break;
			case 0x1C:
			case 0x1D:
			case 0x1E:
				CartridgeMapper = new MBC5(ROMBanks, RAMBanks);
				((MBC5*) CartridgeMapper)->Rumble = true;
				break;
		}
		
		File.seekg(0);
		File.read((char *) ROM.data(), TrueROMSize * 1024llu);
		File.close();
	}
}

Cartridge::~Cartridge() {
	delete CartridgeMapper;
}

bool Cartridge::ReadWord(uint16_t address, uint8_t& data) {
	uint32_t NewAddress;
	if (address >= 0x0000 && address < 0x8000) {
		CartridgeMapper->ROMReadMappedWord(address, NewAddress);
		data = ROM[NewAddress];
		return true;
	} else if (address >= 0xA000 && address < 0xC000) {
		if (CartridgeMapper->RAMReadMappedWord(address, NewAddress)) {
			if ((NewAddress & 0xFFFFFF) == 0xFFFFFF) {
				data = RTCClock[(NewAddress >> 24) - 0x08];
			} else {
				data = ExtRAM[NewAddress];
				if (Header.MapperType == 0x05 || Header.MapperType == 0x06) {
					data |= 0xF0;
				}
			}
		} else {
			data = 0xFF;
		}
		return true;
	}
	return false;
}

bool Cartridge::WriteWord(uint16_t address, uint8_t data) {
	uint32_t NewAddress;
	if (address >= 0x0000 && address < 0x8000) {
		CartridgeMapper->ROMWriteMappedWord(address, data);
		return true;
	} else if (address >= 0xA000 && address < 0xC000) {
		if (CartridgeMapper->RAMWriteMappedWord(address, NewAddress)) {
			if ((NewAddress & 0xFFFFFF) == 0xFFFFFF) {
				RTCClock[(NewAddress >> 24) - 0x08] = data;
			} else {
				ExtRAM[NewAddress] = data;
			}
		}
		return true;
	}
	return false;
}

void Cartridge::TickRTC() {
	if (!(RTCClock[4] & 0x40)) {

		ClockTicks++;

		if (ClockTicks % 128 == 0) {

			RTCClock[0] = (RTCClock[0] + 1) % 60;

			if (RTCClock[0] == 0) {

				RTCClock[1] = (RTCClock[1] + 1) % 60;

				if (RTCClock[1] == 0) {

					RTCClock[2] = (RTCClock[2] + 1) % 24;

					if (RTCClock[2] == 0) {

						RTCClock[3]++;

						if (RTCClock[3] == 0) {

							RTCClock[4] ^= 0x1;

							if ((RTCClock[4] & 0x1) == 0) {

								RTCClock[4] |= 0x80;

							}
						}
					}
				}
			}
		}
	}
}
