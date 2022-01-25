#include "Mapper.hpp"

Mapper::Mapper(uint8_t NumROMBanks, uint8_t NumRAMBanks) {
	ROMBanks = NumROMBanks;
	RAMBanks = NumRAMBanks;
}

Mapper::~Mapper() {

}