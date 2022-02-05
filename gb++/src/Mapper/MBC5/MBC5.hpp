#ifndef MBC5_HPP
#define MBC5_HPP

#include "../Mapper.hpp"

class MBC5 : public Mapper {
	uint8_t RAMEnable = 0x00;
	uint8_t RAMBankNo = 0x00;
	uint16_t ROMBankNo = 0x01;
	uint8_t RumbleEnable = 0x00;
public:
	bool Rumble = false;
	MBC5(uint16_t NumROMBanks, uint16_t NumRAMBanks);
	~MBC5();

	bool ROMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool ROMWriteMappedWord(uint16_t address, uint8_t data) override;

	bool RAMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) override;
};

#endif