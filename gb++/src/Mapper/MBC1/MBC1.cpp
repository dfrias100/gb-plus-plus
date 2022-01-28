#include "MBC1.hpp"

MBC1::MBC1(uint16_t NumROMBanks, uint16_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC1::~MBC1() {
}

bool MBC1::ROMReadMappedWord(uint16_t address, uint32_t& newAddress) {
	if (address <= 0x3FFF) {
		uint32_t Mode1AddressMultiplier = (ROMBankNo & 0x60) % ROMBanks;
		uint32_t Mode1AddressOffset = BankingMode && (ROMBanks > 32) ? 0x4000 * Mode1AddressMultiplier : 0;
		newAddress = address + Mode1AddressOffset;
	} else {
		newAddress = address - 0x4000;
		newAddress += (ROMBankNo % ROMBanks) * 0x4000;
	}
	return true;
}

bool MBC1::ROMWriteMappedWord(uint16_t address, uint8_t data) {
	if (address <= 0x1FFF) {
		RAMEnable = data & 0x0F;
	} else if (address <= 0x3FFF) {
		data = data & 0x1F;
		ROMBankNo &= 0xE0;
		ROMBankNo |= data;

		if ((ROMBankNo & 0x1F) == 0x0)
			ROMBankNo++;
	} else if (address <= 0x5FFF) {
		if (!BankingMode) {
			ROMBankNo &= 0x1F;
			data &= 0x03;
			ROMBankNo |= (data << 5);
			if ((ROMBankNo & 0x1F) == 0x0)
				ROMBankNo++;
		} else {
			RAMBankNo = data & 0x03;
			if (ROMBanks > 32) {
				ROMBankNo &= 0x1F;
				data &= 0x03;
				ROMBankNo |= (data << 5);
				if ((ROMBankNo & 0x1F) == 0x0)
					ROMBankNo++;
			}
		}
	} else if (address <= 0x7FFF) {
		BankingMode = data & 0x03;
	}
	return true;
}

bool MBC1::RAMReadMappedWord(uint16_t address, uint32_t& newAddress) {
	address -= 0xA000;
	if (RAMEnable == 0x0A && RAMBanks > 0) {
		if (BankingMode && ROMBanks < 64 && RAMBanks > 1) {
			newAddress = address + 0x2000 * (RAMBankNo & 0x3);
		} else {
			newAddress = address;
		}
		return true;
	}
	return false;
}

bool MBC1::RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) {
	address -= 0xA000;
	if (RAMEnable == 0x0A && RAMBanks > 0) {
		if (BankingMode && ROMBanks < 64 && RAMBanks > 1) {
			newAddress = address + 0x2000 * (RAMBankNo & 0x3);
		} else {
			newAddress = address;
		}
		return true;
	}
	return false;
}
