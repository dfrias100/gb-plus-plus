#include "MBC0.hpp"

MBC0::MBC0(uint8_t NumROMBanks, uint8_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC0::~MBC0() {
}

bool MBC0::ROMReadMappedWord(uint16_t address, uint16_t& newAddress) {
	newAddress = address;
	return true;
}

bool MBC0::ROMWriteMappedWord(uint16_t address, uint8_t data) {
	return false;
}

bool MBC0::RAMReadMappedWord(uint16_t address, uint16_t& newAddress) {
	return false;
}

bool MBC0::RAMWriteMappedWord(uint16_t address, uint16_t& newAddress) {
	return false;
}

