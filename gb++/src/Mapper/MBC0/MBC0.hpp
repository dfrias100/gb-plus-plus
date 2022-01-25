#ifndef MBC0_HPP
#define MBC0_HPP

#include "../Mapper.hpp"

class MBC0 : public Mapper {
public:
	MBC0(uint8_t NumROMBanks, uint8_t NumRAMBanks);
	~MBC0();

	bool ROMReadMappedWord(uint16_t address, uint16_t &newAddress) override;
	bool ROMWriteMappedWord(uint16_t address, uint16_t &newAddress) override;

	bool RAMReadMappedWord(uint16_t address, uint16_t &newAddress) override;
	bool RAMWriteMappedWord(uint16_t address, uint16_t &newAddress) override;
};	

#endif