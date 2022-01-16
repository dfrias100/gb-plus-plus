#include "Sharp.hpp"
#include "../Memory/Memory.hpp"
#include <iostream>
#include <iomanip>

Sharp::Sharp(Memory* _MemoryBus)
{
	MemoryBus = _MemoryBus;
	PC = 0x100;
	Suspended = false;
}

void Sharp::SetFlag(SharpFlags flag, bool set) {
	if (set) {
		F |= flag;
	}
	else {
		F &= ~flag;
	}
}

uint8_t Sharp::GetFlag(SharpFlags flag) {
	return (F & flag) > 0 ? 1 : 0;
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

void Sharp::RotateLeftCircular(uint8_t& arg) {
	SetFlag(c, (arg & 0x80) >> 7);

	arg <<= 1;
	arg += GetFlag(c);

	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::RotateRightCircular(uint8_t& arg) {
	SetFlag(c, arg & 0x1);

	arg >>= 1;
	arg += (GetFlag(c) << 7);

	if (arg == 0) {
		SetFlag(z, 1);
	}
	else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::RotateLeft(uint8_t& arg) {
	temp = GetFlag(c);
	SetFlag(c, (arg & 0x80) >> 7);

	arg <<= 1;
	arg += temp;

	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::RotateRight(uint8_t& arg) {
	temp = GetFlag(c);
	SetFlag(c, arg & 0x1);

	arg >>= 1;
	arg += (temp << 7);

	if (arg == 0) {
		SetFlag(z, 1);
	}
	else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

// Non 0xCB instructions
void Sharp::NOP() {
}

void Sharp::LD_BC_DW() {
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

void Sharp::LD_B_W() {
	B = (uint8_t) CurrOperand;
}

void Sharp::RLCA() {
	RotateLeftCircular(A);
	SetFlag(z, 0);
}

void Sharp::LD_ADDR_DW_SP() {
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

void Sharp::DEC_C() {
	DecrementRegister(C);
}

void Sharp::LD_C_W() {
	C = (uint8_t)CurrOperand;
}

void Sharp::RRCA() {
	RotateRightCircular(A);
	SetFlag(z, 0);
}

void Sharp::STOP() {
	Suspended = true;
}

void Sharp::LD_DE_DW() {
	DE = CurrOperand;
}

void Sharp::LD_ADDR_DE_A() {
	MemoryBus->CPUWrite(DE, A);
}

void Sharp::INC_DE(){
	DE++;
}

void Sharp::INC_D() {
	IncrementRegister(D);
}

void Sharp::DEC_D() {
	DecrementRegister(D);
}

void Sharp::LD_D_W() {
	D = (uint8_t) CurrOperand;
}

void Sharp::RLA() {
	RotateLeft(A);
}

void Sharp::JR_SW() {
	PC = PC + (int8_t) (CurrOperand);
}

void Sharp::ADD_HL_DE() {
	UnsignedAdd16(HL, DE);
}

void Sharp::LD_A_ADDR_DE() {
	A = MemoryBus->CPURead(DE);
}

void Sharp::DEC_DE() {
	DE--;
}

void Sharp::INC_E() {
	IncrementRegister(E);
}

void Sharp::DEC_E() {
	DecrementRegister(E);
}

void Sharp::LD_E_W() {
	E = (uint8_t) CurrOperand;
}

void Sharp::RRA() {
	RotateRight(A);
}

void Sharp::JR_NZ_SW() {
	if (!GetFlag(z)) {
		PC += (int8_t) CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::LD_HL_DW() {
	HL = CurrOperand;
}

void Sharp::LD_ADDR_HL_PI_A() {
	MemoryBus->CPUWrite(HL++, A);
}

void Sharp::INC_HL() {
	HL++;
}

void Sharp::INC_H() {
	IncrementRegister(H);
}

void Sharp::DEC_H() {
	DecrementRegister(H);
}

void Sharp::LD_H_W() {
	H = (uint8_t)CurrOperand;
}

void Sharp::DAA() {
	temp = 0x0;

	if (GetFlag(h) || (!GetFlag(n) && ((A & 0xF) > 0x9))) {
		temp |= 0x06;
	}

	if (GetFlag(c) || (!GetFlag(n) && (A > 0x99))) {
		temp |= 0x60;
		// Set the flag to 1 if and only if the previous instruction
		// Was an addition and either:
		//	- A carry occurred
		//  - Register A was bigger than 0x99
		SetFlag(c, GetFlag(c) | (!GetFlag(n) && (GetFlag(c) || (A > 0x99)) ) );
	}

	temp += GetFlag(n) ? -temp : temp;

	if (A == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(h, 0);
}

void Sharp::JR_Z_SW() {
	if (GetFlag(z)) {
		PC += (int8_t) CurrOperand;
	}
}

void Sharp::ADD_HL_HL() {
	UnsignedAdd16(HL, HL);
}

void Sharp::LD_A_ADDR_HL_PI() {
	A = MemoryBus->CPURead(HL++);
}

void Sharp::DEC_HL() {
	HL--;
}

void Sharp::INC_L() {
	IncrementRegister(L);
}

void Sharp::DEC_L() {
	DecrementRegister(L);
}

void Sharp::LD_L_W() {
	L = CurrOperand;
}

void Sharp::CPL() {
	A = ~A;

	SetFlag(n, 1);
	SetFlag(h, 1);
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
