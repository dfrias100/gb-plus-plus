#include "Sharp.hpp"
#include "../Memory/Memory.hpp"
#include <iostream>

Sharp::Sharp(Memory* _MemoryBus)
{
	MemoryBus = _MemoryBus;
	PC = 0x100;
}

void Sharp::SetFlag(SharpFlags flag, bool set) {
	if (set) {
		Flags |= flag;
	}
	else {
		Flags &= ~flag;
	}
}

uint8_t Sharp::GetFlag(SharpFlags flag) {
	return (Flags & flag) > 0 ? 1 : 0;
}

// Helper Functions
void Sharp::DecrementRegister(uint8_t& reg) {
	if ((reg & 0xF) > 0) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	B--;

	if (B == 0) {
		SetFlag(z, 1);
	}
	else {
		SetFlag(z, 0);
	}

	SetFlag(n, 1);
}

void Sharp::IncrementRegister(uint8_t& reg) {
	if (reg & 0xF) {
		SetFlag(h, 1);
	}
	else {
		SetFlag(h, 0);
	}

	reg++;

	if (reg == 0) {
		SetFlag(z, 1);
	}
	else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
}

void Sharp::UnsignedAdd16(uint16_t& dest, uint16_t src) {
	if (src > (0xFFFF - dest)) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	temp = (dest & 0x0FFF) + (src & 0x0FFF);

	if ((temp & 0x1000) == 0x1000) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	dest += src;

	SetFlag(n, 0);
}

// Non 0xCB instructions
void Sharp::NOP() {
}

void Sharp::LD_BC_NN() {
	BC = CurrOperand;
}

void Sharp::LD_ADDR_BC_A() {
	MemoryBus->CPUWrite(BC, A);
}

void Sharp::INC_BC() {
	BC++;
}

void Sharp::INC_B() {
	IncrementRegister(B);
}

void Sharp::DEC_B() {
	DecrementRegister(B);
}

void Sharp::LD_B_N() {
	B = (uint8_t) CurrOperand;
}

void Sharp::RCLA() {
	SetFlag(c, (A & 0x80) >> 7);

	A <<= 1;
	A += GetFlag(c);

	if (A == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}
}

void Sharp::LD_ADDR_NN_SP() {
	MemoryBus->CPUWrite16(CurrOperand, SP);
}

void Sharp::ADD_HL_BC() {
	UnsignedAdd16(HL, BC);
}

void Sharp::LD_A_ADDR_BC() {
	A = MemoryBus->CPURead(BC);
}

void Sharp::DEC_BC() {
	BC--;
}

void Sharp::INC_C() {
	IncrementRegister(C);
}

void Sharp::LD_C_N() {
	C = (uint8_t)CurrOperand;
}

Sharp::~Sharp() {
}

void Sharp::Clock() {
	if (CurrCycles == 0) {
		Opcode = MemoryBus->CPURead(PC++);
		DecodedInstr = SHARPINSTRS[Opcode];

		CurrCycles = DecodedInstr.Cycles;
		CurrArgSize = DecodedInstr.ArgSize;

		//CurrCycles = SHARPINSTRS[Opcode].Cycles;
		//CurrArgSize = SHARPINSTRS[Opcode].ArgSize;

		switch (CurrArgSize) {
			case 1:
				CurrOperand = (uint16_t) MemoryBus->CPURead(PC++);
				break;
			case 2:
				CurrOperand = MemoryBus->CPURead16(PC);
				PC += 2;
				break;
		}

		(this->*DecodedInstr.Instruction)();
	}
	CurrCycles--;
}
