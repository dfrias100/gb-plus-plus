#ifndef MBC3_HPP
#define MBC3_HPP

#include "../Mapper.hpp"

class MBC3 : public Mapper {
	uint8_t RAMRTCEnable = 0x00;
	uint8_t RTCRegister = 0x08;
	uint16_t ROMBankNo = 0x01;
	uint8_t RAMBankNo = 0x00;
	uint8_t BankingMode = 0x01;
public:
	MBC3(uint16_t NumROMBanks, uint16_t NumRAMBanks);
	~MBC3();

	bool ROMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool ROMWriteMappedWord(uint16_t address, uint8_t data) override;

	bool RAMReadMappedWord(uint16_t address, uint32_t& newAddress) override;
	bool RAMWriteMappedWord(uint16_t address, uint32_t& newAddress) override;
};

#endif