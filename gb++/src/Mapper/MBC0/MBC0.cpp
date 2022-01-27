#include "MBC0.hpp"

MBC0::MBC0(uint16_t NumROMBanks, uint16_t NumRAMBanks) : Mapper(NumROMBanks, NumRAMBanks) {
}

MBC0::~MBC0() {
}

bool MBC0::ROMReadMappedWord(uint16_t address, uint32_t& newAddress) {
	newAddress = address;
	return true;
}

bool MBC0::ROMWriteMappedWord(uint16_t address, uint8_t data) {
	return false;
}

bool MBC0::RAMReadMappedWord(uint16_t address, uint32_t& newAddress) {
	return false;
}

bool MBC0::RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) {
	return false;
}

