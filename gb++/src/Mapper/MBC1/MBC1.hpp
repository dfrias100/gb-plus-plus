#ifndef MBC1_HPP
#define MBC1_HPP

#include "../Mapper.hpp"

class MBC1 : public Mapper {
	uint8_t RAMEnable = 0x00;
	uint16_t ROMBankNo = 0x01;
	uint8_t RAMBankNo = 0x00;
	uint8_t BankingMode = 0x00;
public:
	MBC1(uint16_t NumROMBanks, uint16_t NumRAMBanks);
	~MBC1();

	bool ROMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool ROMWriteMappedWord(uint16_t address, uint8_t data) override;

	bool RAMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) override;
};

#endif