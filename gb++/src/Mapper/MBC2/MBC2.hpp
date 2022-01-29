#ifndef MBC2_HPP
#define MBC2_HPP

#include "../Mapper.hpp"

class MBC2 : public Mapper {
	uint16_t ROMBankNo = 0x01;
	uint8_t RAMEnable = 0x00;
public:
	MBC2(uint16_t NumROMBanks, uint16_t NumRAMBanks);
	~MBC2();

	bool ROMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool ROMWriteMappedWord(uint16_t address, uint8_t data) override;

	bool RAMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) override;
};

#endif