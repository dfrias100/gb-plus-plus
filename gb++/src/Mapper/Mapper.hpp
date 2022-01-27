#ifndef MAPPER_HPP
#define MAPPER_HPP

#include <cstdint>

class Mapper {
public:
	Mapper(uint16_t NumROMBanks, uint16_t NumRAMBanks);
	virtual ~Mapper();

	virtual bool ROMReadMappedWord(uint16_t address, uint32_t& newAddress) = 0;
	virtual bool ROMWriteMappedWord(uint16_t address, uint8_t data) = 0;
		
	virtual bool RAMReadMappedWord(uint16_t address, uint32_t& newAddress) = 0;
	virtual bool RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) = 0;
protected:
	uint16_t ROMBanks;
	uint16_t RAMBanks;
};

#endif