#include "MBC2.hpp"

MBC2::MBC2(uint16_t NumROMBanks, uint16_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC2::~MBC2() {
}

bool MBC2::ROMReadMappedWord(uint16_t address, uint32_t& newAddress) {
	if (address <= 0x3FFF) {
		newAddress = address;
	} else {
		newAddress = (address - 0x4000) + (ROMBankNo % ROMBanks) * 0x4000;
	}
	return true;
}

bool MBC2::ROMWriteMappedWord(uint16_t address, uint8_t data) {
	if (address <= 0x3FFF) {
		if (address & 0x100) {
			ROMBankNo = data & 0x0F;
			if (ROMBankNo == 0)
				ROMBankNo++;
		} else {
			RAMEnable = data & 0x0F;
		}
		return true;
	}
	return false;
}

bool MBC2::RAMReadMappedWord(uint16_t address, uint32_t& newAddress) {
	if (RAMEnable == 0x0A) {
		if (address <= 0xA1FF) {
			newAddress = address - 0xA000;
		} else {
			newAddress = (address - 0xA000) & 0x1FF;
		}
		return true;
	}
	return false;
}

bool MBC2::RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) {
	if (RAMEnable == 0x0A) {
		if (address <= 0xA1FF) {
			newAddress = address - 0xA000;
		} else {
			newAddress = (address - 0xA000) & 0x1FF;
		}
		return true;
	}
	return false;
}
