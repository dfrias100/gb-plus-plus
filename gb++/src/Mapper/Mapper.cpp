#include "Mapper.hpp"

Mapper::Mapper(uint16_t NumROMBanks, uint16_t NumRAMBanks) {
	ROMBanks = NumROMBanks;
	RAMBanks = NumRAMBanks;
}

Mapper::~Mapper() {

}