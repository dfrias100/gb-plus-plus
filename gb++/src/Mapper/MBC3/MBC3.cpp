#include "MBC3.hpp"

MBC3::MBC3(uint16_t NumROMBanks, uint16_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC3::~MBC3() {
}

bool MBC3::ROMReadMappedWord(uint16_t address, uint32_t& newAddress) {
    if (address <= 0x3FFF) {
        newAddress = address;
    } else {
        newAddress = address - 0x4000;
        newAddress += (ROMBankNo % ROMBanks) * 0x4000;
    }
    return true;
}

bool MBC3::ROMWriteMappedWord(uint16_t address, uint8_t data) {
    if (address <= 0x1FFF) {
        RAMRTCEnable = data & 0x0F;
    } else if (address <= 0x3FFF) {
        ROMBankNo = data & 0x7F;
        if (ROMBankNo == 0)
            ROMBankNo++;
    } else if (address <= 0x5FFF) {
        if (data <= 0x03) {
            BankingMode = 0x1;
            RAMBankNo = data;
        } else if (data >= 0x08 && data <= 0x0C) {
            BankingMode = 0x0;
            RTCRegister = data;
        }
    }
    return true;
}

bool MBC3::RAMReadMappedWord(uint16_t address, uint32_t& newAddress) {
    address -= 0xA000;
    if (RAMRTCEnable == 0x0A) {
        if (BankingMode == 0x01 && RAMBanks > 1) {
            newAddress = address + 0x2000 * RAMBankNo;
            return true;
        } else if (BankingMode == 0x00) {
            newAddress = (RTCRegister << 24) | 0xFFFFFF;
            return true;
        } else if (RAMBanks == 1) {
            newAddress = address;
        } else {
            return false;
        }
    }
    return false;
}

bool MBC3::RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) {
    address -= 0xA000;
    if (RAMRTCEnable == 0x0A) {
        if (BankingMode == 0x01 && RAMBanks > 1) {
            newAddress = address + 0x2000 * RAMBankNo;
            return true;
        } else if (BankingMode == 0x00) {
            newAddress = (RTCRegister << 24) | 0xFFFFFF;
            return true;
        } else {
            return false;
        }
    }
    return false;
}
