#include "MBC5.hpp"

MBC5::MBC5(uint16_t NumROMBanks, uint16_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC5::~MBC5() {
}

bool MBC5::ROMReadMappedWord(uint16_t address, uint32_t& newAddress) {
    if (address <= 0x3FFF) {
        newAddress = address;
    } else {
        newAddress = address - 0x4000;
        newAddress += (ROMBankNo % ROMBanks) * 0x4000;
    }
    return true;
}

bool MBC5::ROMWriteMappedWord(uint16_t address, uint8_t data) {
    if (address <= 0x1FFF) {
        RAMEnable = data & 0x0F;
    } else if (address <= 0x2FFF) {
        ROMBankNo = data;
    } else if (address <= 0x3FFF) {
        ROMBankNo |= (data & 0x01) << 8;
    } else if (address <= 0x5FFF) {
        if (data <= 0x0F) {
            RAMBankNo = data;
        }

        if (Rumble) {
            RumbleEnable = (RAMBankNo & 0x08) >> 3;
            RAMBankNo &= ~0x08;
        }
    }
    return true;
}

bool MBC5::RAMReadMappedWord(uint16_t address, uint32_t& newAddress) {
    address -= 0xA000;
    if (RAMEnable == 0x0A && RAMBanks > 0) {
        if (RAMBanks > 1) {
            newAddress = address + 0x2000 * RAMBankNo;
        } else {
            newAddress = address;
        }
        return true;
    }
	return false;
}

bool MBC5::RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) {
    address -= 0xA000;
    if (RAMEnable == 0x0A && RAMBanks > 0) {
        if (RAMBanks > 1) {
            newAddress = address + 0x2000 * RAMBankNo;
        }
        else {
            newAddress = address;
        }
        return true;
    }
    return false;
}
