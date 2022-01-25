#ifndef MAPPER_HPP
#define MAPPER_HPP

#include <cstdint>

class Mapper {
public:
	Mapper(uint8_t NumROMBanks, uint8_t NumRAMBanks);
	virtual ~Mapper();

	virtual bool ROMReadMappedWord(uint16_t address, uint16_t& newAddress) = 0;
	virtual bool ROMWriteMappedWord(uint16_t address, uint8_t data) = 0;
		
	virtual bool RAMReadMappedWord(uint16_t address, uint16_t& newAddress) = 0;
	virtual bool RAMWriteMappedWord(uint16_t address, uint16_t& newAddress) = 0;
protected:
	uint8_t ROMBanks;
	uint8_t RAMBanks;
};

#endif