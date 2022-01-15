#include "Memory.hpp"

Memory::Memory() {
	HighRAM	   = new uint8_t[127];
	IO		   = new uint8_t[76];
	SpriteOAM  = new uint8_t[160];
	WorkingRAM = new uint8_t[1024 * 8];
	SwitchRAM  = new uint8_t[1024 * 8];
	VideoRAM   = new uint8_t[1024 * 2];
	Cartridge  = new uint8_t[1024 * 32];

	InterruptEnableRegister = 0x00;

	CPU = new Sharp(this);
}

Memory::~Memory()
{
	delete[] HighRAM;
	delete[] IO;
	delete[] SpriteOAM;
	delete[] WorkingRAM;
	delete[] SwitchRAM;
	delete[] VideoRAM;
	delete[] Cartridge;

	delete CPU;
}

void Memory::CPUWrite(uint16_t address, uint8_t data)
{
}

uint8_t Memory::CPURead(uint16_t address)
{
	return uint8_t();
}

void Memory::CPUWrite16(uint16_t address, uint16_t data)
{
}

uint16_t Memory::CPURead16(uint16_t address)
{
	return uint16_t();
}

void Memory::CPUWriteStack(uint16_t address, uint16_t data)
{
}

uint16_t Memory::CPUReadStack(uint16_t address)
{
	return uint16_t();
}


void Memory::Clock() {
	CPU->Clock();
}

