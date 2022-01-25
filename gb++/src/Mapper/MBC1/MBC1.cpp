#include "MBC1.hpp"

MBC1::MBC1(uint16_t NumROMBanks, uint16_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC1::~MBC1() {
}

bool MBC1::ROMReadMappedWord(uint16_t address, uint16_t& newAddress) {
	uint16_t LargeROMOffset = BankingMode && (ROMBanks > 32) ? 0x4000 * (ROMBankNo & 0x70) : 0;
	newAddress = LargeROMOffset;
	if (address <= 0x3FFF) {
		newAddress += address;
	} else {
		newAddress += address + 0x4000 * ((ROMBankNo & 0x1F) - 1);
	}
	return true;
}

bool MBC1::ROMWriteMappedWord(uint16_t address, uint8_t data) {
	if (address <= 0x1FFF) {
		RAMEnable = data & 0x0F;
	} else if (address <= 0x3FFF) {
		if (ROMBanks < 64) {
			ROMBankNo = data & (ROMBanks - 1);
		} else {
			ROMBankNo = (data & (ROMBanks - 1)) + (RAMBankNo << 5);
		}

		switch (ROMBankNo) {
			case 0x00:
			case 0x20:
			case 0x40:
			case 0x60:
				ROMBankNo++;
				break;
		}
	} else if (address <= 0x5FFF) {
		RAMBankNo = data & 0x03;
	} else if (address <= 0x7FFF) {
		BankingMode = data & 0x03;
	}
	return true;
}

bool MBC1::RAMReadMappedWord(uint16_t address, uint16_t& newAddress) {
	return false;
}

bool MBC1::RAMWriteMappedWord(uint16_t address, uint16_t& newAddress) {
	return false;
}
