#ifndef MBC0_HPP
#define MBC0_HPP

#include "../Mapper.hpp"

class MBC0 : public Mapper {
public:
	MBC0(uint16_t NumROMBanks, uint16_t NumRAMBanks);
	~MBC0();

	bool ROMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool ROMWriteMappedWord(uint16_t address, uint8_t data) override;

	bool RAMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) override;
};	

#endif