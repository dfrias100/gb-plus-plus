#include "Sharp.hpp"
#include "../Memory/Memory.hpp"
#include <iostream>
#include <iomanip>

Sharp::Sharp(Memory* _MemoryBus) {
	MemoryBus = _MemoryBus;
	PC = 0x100;
	Suspended = false;
}

void Sharp::SetFlag(SharpFlags flag, bool set) {
	if (set) {
		F |= flag;
	} else {
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
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 1);
}

void Sharp::IncrementRegister(uint8_t& reg) {
	if (reg & 0xF) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	reg++;

	if (reg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
}

void Sharp::UnsignedAdd(uint8_t& dest, uint8_t src) {
	if (src > (0xFF - dest)) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	temp = (dest & 0xF) + (src & 0xF);

	if ((temp & 0x10) == 0x10) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	dest += src;

	if (dest == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
}

void Sharp::UnsignedAddCarry(uint8_t& dest, uint8_t src) {
	temp = GetFlag(c);

	if (src > (0xFF - dest) || (temp && (src + dest) == 0xFF)) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	temp2 = (dest & 0xF) + (src & 0xF);

	if ((temp2 & 0x10) == 0x10) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	dest += src + temp;

	if (dest == 0) {
		SetFlag(z, 1);
	} else {
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

	temp3 = (dest & 0x0FFF) + (src & 0x0FFF);

	if ((temp3 & 0x1000) == 0x1000) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	dest += src;

	SetFlag(n, 0);
}

void Sharp::Subtract(uint8_t& dest, uint8_t src) {
	if (src < dest) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	if ((src & 0xF) < (dest & 0xF)) {
		SetFlag(h, 1); 
	} else {
		SetFlag(h, 0);
	}

	dest -= src;

	if (dest == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}
	
	SetFlag(n, 1);
}

void Sharp::SubtractWithCarry(uint8_t& dest, uint8_t src) {
	temp = GetFlag(c);

	if (src < dest || (temp && (src - dest) > 0x0)) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	if ((src & 0xF) < (dest & 0xF)) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	dest -= (src + temp);

	if (dest == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 1);
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

void Sharp::ShiftLeft(uint8_t& arg) {
	SetFlag(c, (arg & 0x80) >> 7);

	arg <<= 1;

	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::ShiftRightArithmetic(uint8_t& arg) {
	SetFlag(c, arg & 0x01);

	arg = (arg & 0x80) | (arg >> 1);

	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::ShiftRight(uint8_t& arg) {
	SetFlag(c, arg & 0x01);

	arg >>= 1;

	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::Swap(uint8_t& arg) {
	temp = arg & 0xF;
	arg &= 0xF0;
	arg >>= 4;
	arg |= (temp << 4);


	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
	SetFlag(c, 0);
}

void Sharp::And(uint8_t arg) {
	A &= arg;

	if (A == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 1);
	SetFlag(c, 0);
}

void Sharp::Xor(uint8_t arg) {
	A ^= arg;

	if (A == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
	SetFlag(c, 0);
}

void Sharp::Or(uint8_t arg) {
	A |= arg;

	if (A == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
	SetFlag(c, 0);
}

void Sharp::Bit(uint8_t pos, uint8_t arg) {
	SetFlag(z, ~(arg >> pos) & 0x1);
	SetFlag(h, 0);
	SetFlag(n, 1);
}

void Sharp::UNOP() {
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
	C = (uint8_t) CurrOperand;
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
	H = (uint8_t) CurrOperand;
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
		CurrCycles += 4;
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
	L = (uint8_t) CurrOperand;
}

void Sharp::CPL() {
	A = ~A;

	SetFlag(n, 1);
	SetFlag(h, 1);
}

void Sharp::JR_NC_SW() {
	if (!GetFlag(c)) {
		PC += (int8_t) CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::LD_SP_DW() {
	SP = CurrOperand;
}

void Sharp::LD_ADDR_HL_PD_A() {
	MemoryBus->CPUWrite(HL--, A);
}

void Sharp::INC_SP() {
	SP++;
}

void Sharp::INC_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	IncrementRegister(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::DEC_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	DecrementRegister(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::LD_ADDR_HL_W() {
	MemoryBus->CPUWrite(HL, (uint8_t) CurrOperand);
}

void Sharp::SCF() {
	SetFlag(n, 0);
	SetFlag(h, 0);
	SetFlag(c, 1);
}

void Sharp::JR_C_SW() {
	if (GetFlag(c)) {
		PC += (int8_t) CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::ADD_HL_SP() {
	UnsignedAdd16(HL, SP);
}

void Sharp::LD_A_ADDR_HL_PD() {
	A = MemoryBus->CPURead(HL--);
}

void Sharp::DEC_SP() {
	SP--;
}

void Sharp::INC_A() {
	IncrementRegister(A);
}

void Sharp::DEC_A() {
	DecrementRegister(A);
}

void Sharp::LD_A_W() {
	A = (uint8_t) CurrOperand;
}

void Sharp::CCF() {
	SetFlag(n, 0);
	SetFlag(h, 0);
	SetFlag(c, !GetFlag(c));
}

void Sharp::LD_B_B() {
}

void Sharp::LD_B_C() {
	B = C;
}

void Sharp::LD_B_D() {
	B = D;
}

void Sharp::LD_B_E() {
	B = E;
}

void Sharp::LD_B_H() {
	B = H;
}

void Sharp::LD_B_L() {
	B = L;
}

void Sharp::LD_B_ADDR_HL() {
	B = MemoryBus->CPURead(HL);
}

void Sharp::LD_B_A() {
	B = A;
}

void Sharp::LD_C_B() {
	C = B;
}

void Sharp::LD_C_C() {
}

void Sharp::LD_C_D() {
	C = D;
}

void Sharp::LD_C_E() {
	C = E;
}

void Sharp::LD_C_H() {
	C = H;
}

void Sharp::LD_C_L() {
	C = L;
}

void Sharp::LD_C_ADDR_HL() {
	C = MemoryBus->CPURead(HL);
}

void Sharp::LD_C_A() {
	C = A;
}

void Sharp::LD_D_B() {
	D = B;
}

void Sharp::LD_D_C() {
	D = C;
}

void Sharp::LD_D_D() {
}

void Sharp::LD_D_E() {
	D = E;
}

void Sharp::LD_D_H() {
	D = H;
}

void Sharp::LD_D_L() {
	D = L;
}

void Sharp::LD_D_ADDR_HL() {
	D = MemoryBus->CPURead(HL);
}

void Sharp::LD_D_A() {
	D = A;
}

void Sharp::LD_E_B() {
	E = B;
}

void Sharp::LD_E_C() {
	E = C;
}

void Sharp::LD_E_D() {
	E = D;
}

void Sharp::LD_E_E() {
}

void Sharp::LD_E_H() {
	E = H;
}

void Sharp::LD_E_L() {
	E = L;
}

void Sharp::LD_E_ADDR_HL() {
	E = MemoryBus->CPURead(HL);
}

void Sharp::LD_E_A() {
	E = A;
}

void Sharp::LD_H_B() {
	H = B;
}

void Sharp::LD_H_C() {
	H = C;
}

void Sharp::LD_H_D() {
	H = D;
}

void Sharp::LD_H_E() {
	H = E;
}

void Sharp::LD_H_H() {
}

void Sharp::LD_H_L() {
	H = L;
}

void Sharp::LD_H_ADDR_HL() {
	H = MemoryBus->CPURead(HL);
}

void Sharp::LD_H_A() {
	H = A;
}

void Sharp::LD_L_B() {
	L = B;
}

void Sharp::LD_L_C() {
	L = C;
}

void Sharp::LD_L_D() {
	L = D;
}

void Sharp::LD_L_E() {
	L = E;
}

void Sharp::LD_L_H() {
	L = H;
}

void Sharp::LD_L_L() {
}

void Sharp::LD_L_ADDR_HL() {
	L = MemoryBus->CPURead(HL);
}

void Sharp::LD_L_A() {
	L = A;
}

void Sharp::LD_ADDR_HL_B() {
	MemoryBus->CPUWrite(HL, B);
}

void Sharp::LD_ADDR_HL_C() {
	MemoryBus->CPUWrite(HL, C);
}

void Sharp::LD_ADDR_HL_D() {
	MemoryBus->CPUWrite(HL, D);
}

void Sharp::LD_ADDR_HL_E() {
	MemoryBus->CPUWrite(HL, E);
}

void Sharp::LD_ADDR_HL_H() {
	MemoryBus->CPUWrite(HL, H);
}

void Sharp::LD_ADDR_HL_L() {
	MemoryBus->CPUWrite(HL, L);
}

// TODO: Implement this halt
void Sharp::HALT() {
	if (InterruptMasterEnable) {

	} else {
		PC++;
	}
}

void Sharp::LD_ADDR_HL_A() {
	MemoryBus->CPUWrite(HL, A);
}

void Sharp::LD_A_B() {
	A = B;
}

void Sharp::LD_A_C() {
	A = C;
}

void Sharp::LD_A_D() {
	A = D;
}

void Sharp::LD_A_E() {
	A = E;
}

void Sharp::LD_A_H() {
	A = H;
}

void Sharp::LD_A_L() {
	A = L;
}

void Sharp::LD_A_ADDR_HL() {
	A = MemoryBus->CPURead(HL);
}

void Sharp::LD_A_A() {
}

void Sharp::ADD_A_B() {
	UnsignedAdd(A, B);
}

void Sharp::ADD_A_C() {
	UnsignedAdd(A, C);
}

void Sharp::ADD_A_D() {
	UnsignedAdd(A, D);
}

void Sharp::ADD_A_E() {
	UnsignedAdd(A, E);
}

void Sharp::ADD_A_H() {
	UnsignedAdd(A, H);
}

void Sharp::ADD_A_L() {
	UnsignedAdd(A, L);
}

void Sharp::ADD_A_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	UnsignedAdd(A, temp);
}

void Sharp::ADD_A_A() {
	UnsignedAdd(A, A);
}

void Sharp::ADC_A_B() {
	UnsignedAddCarry(A, B);
}

void Sharp::ADC_A_C() {
	UnsignedAddCarry(A, C);
}

void Sharp::ADC_A_D() {
	UnsignedAddCarry(A, D);
}

void Sharp::ADC_A_E() {
	UnsignedAddCarry(A, E);
}

void Sharp::ADC_A_H() {
	UnsignedAddCarry(A, H);
}

void Sharp::ADC_A_L() {
	UnsignedAddCarry(A, L);
}

void Sharp::ADC_A_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	UnsignedAddCarry(A, temp);
}

void Sharp::ADC_A_A() {
	UnsignedAddCarry(A, A);
}

void Sharp::SUB_B() {
	Subtract(A, B);
}

void Sharp::SUB_C() {
	Subtract(A, C);
}

void Sharp::SUB_D() {
	Subtract(A, D);
}

void Sharp::SUB_E() {
	Subtract(A, E);
}

void Sharp::SUB_H() {
	Subtract(A, H);
}

void Sharp::SUB_L() {
	Subtract(A, L);
}

void Sharp::SUB_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	Subtract(A, temp);
}

void Sharp::SUB_A() {
	Subtract(A, A);
}

void Sharp::SBC_A_B() {
	SubtractWithCarry(A, B);
}

void Sharp::SBC_A_C() {
	SubtractWithCarry(A, C);
}

void Sharp::SBC_A_D() {
	SubtractWithCarry(A, D);
}

void Sharp::SBC_A_E() {
	SubtractWithCarry(A, E);
}

void Sharp::SBC_A_H() {
	SubtractWithCarry(A, H);
}

void Sharp::SBC_A_L() {
	SubtractWithCarry(A, L);
}

void Sharp::SBC_A_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	SubtractWithCarry(A, temp);
}

void Sharp::SBC_A_A() {
	SubtractWithCarry(A, A);
}

void Sharp::AND_B() {
	And(B);
}

void Sharp::AND_C() {
	And(C);
}

void Sharp::AND_D() {
	And(D);
}

void Sharp::AND_E() {
	And(E);
}

void Sharp::AND_H() {
	And(H);
}

void Sharp::AND_L() {
	And(L);
}

void Sharp::AND_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	And(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::AND_A() {
	And(A);
}

void Sharp::XOR_B() {
	Xor(B);
}

void Sharp::XOR_C() {
	Xor(C);
}

void Sharp::XOR_D() {
	Xor(D);
}

void Sharp::XOR_E() {
	Xor(E);
}

void Sharp::XOR_H() {
	Xor(H);
}

void Sharp::XOR_L() {
	Xor(L);
}

void Sharp::XOR_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	Xor(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::XOR_A() {
	Xor(A);
}

void Sharp::OR_B() {
	Or(B);
}

void Sharp::OR_C() {
	Or(C);
}

void Sharp::OR_D() {
	Or(D);
}

void Sharp::OR_E() {
	Or(E);
}

void Sharp::OR_H() {
	Or(H);
}

void Sharp::OR_L() {
	Or(L);
}

void Sharp::OR_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	Or(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::OR_A() {
	Or(A);
}

void Sharp::CP_B() {
	temp = A;
	Subtract(temp, B);
}

void Sharp::CP_C() {
	temp = A;
	Subtract(temp, C);
}

void Sharp::CP_D() {
	temp = A;
	Subtract(temp, D);
}

void Sharp::CP_E() {
	temp = A;
	Subtract(temp, E);
}

void Sharp::CP_H() {
	temp = A;
	Subtract(temp, H);
}

void Sharp::CP_L() {
	temp = A;
	Subtract(temp, L);
}

void Sharp::CP_ADDR_HL() {
	temp = A;
	temp2 = MemoryBus->CPURead(HL);
	Subtract(temp, temp2);
}

void Sharp::CP_A() {
	temp = A;
	Subtract(temp, A);
}

void Sharp::RET_NZ() {
	if (!GetFlag(z)) {
		PC = MemoryBus->CPURead16(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::POP_BC() {
	BC = MemoryBus->CPURead16(SP);
	SP += 2;
}

void Sharp::JP_NZ_ADDR_DW() {
	if (!GetFlag(z)) {
		PC = CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::JP_ADDR_DW() {
	PC = CurrOperand;
}

void Sharp::CALL_NZ_ADDR_DW() {
	if (!GetFlag(z)) {
		SP -= 2;
		MemoryBus->CPUWrite16(SP, PC);
		PC = CurrOperand;
		CurrCycles += 12;
	}
}

void Sharp::PUSH_BC() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, BC);
}

void Sharp::ADD_A_W() {
	UnsignedAdd(A, (uint8_t) CurrOperand);
}

void Sharp::RST_00H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0000;
}

void Sharp::RET_Z() {
	if (GetFlag(z)) {
		PC = MemoryBus->CPURead16(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::RET() {
	PC = MemoryBus->CPURead16(SP);
	SP += 2;
}

void Sharp::JP_Z_ADDR_DW() {
	if (GetFlag(z)) {
		PC = CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::PREFIX_CB() {
	Opcode = MemoryBus->CPURead(PC++);
	DecodedInstr = SHARPINSTRS_CB[Opcode];

	CurrCycles = DecodedInstr.Cycles;
	CurrArgSize = DecodedInstr.ArgSize;

	(this->*DecodedInstr.Instruction)();
}

void Sharp::CALL_Z_ADDR_DW() {
	if (GetFlag(z)) {
		SP -= 2;
		MemoryBus->CPUWrite16(SP, PC);
		PC = CurrOperand;
		CurrCycles += 12;
	}
}

void Sharp::CALL_ADDR_DW() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = CurrOperand;
}

void Sharp::ADC_A_W() {
	UnsignedAdd(A, (uint8_t) CurrOperand);
}

void Sharp::RST_08H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0008;
}

void Sharp::RET_NC() {
	if (!GetFlag(c)) {
		PC = MemoryBus->CPURead16(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::POP_DE() {
	DE = MemoryBus->CPURead16(SP);
	SP += 2;
}

void Sharp::JP_NC_ADDR_DW() {
	if (!GetFlag(c)) {
		PC = CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::CALL_NC_ADDR_DW() {
if (!GetFlag(c)) {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = CurrOperand;
	CurrCycles += 12;
}
}

void Sharp::PUSH_DE() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, DE);
}

void Sharp::SUB_W() {
	Subtract(A, (uint8_t)CurrOperand);
}

void Sharp::RST_10H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0010;
}

void Sharp::RET_C() {
	if (GetFlag(c)) {
		PC = MemoryBus->CPURead16(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::RETI() {
	PC = MemoryBus->CPURead16(SP);
	SP += 2;
	InterruptMasterEnable = 0x1;
}

void Sharp::JP_C_ADDR_DW() {
	if (GetFlag(c)) {
		PC = CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::CALL_C_ADDR_DW() {
	if (GetFlag(c)) {
		SP -= 2;
		MemoryBus->CPUWrite16(SP, PC);
		PC = CurrOperand;
		CurrCycles += 12;
	}
}

void Sharp::SBC_A_W() {
	SubtractWithCarry(A, (uint8_t) CurrOperand);
}

void Sharp::RST_18H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0018;
}

void Sharp::LDH_ADDR_W_A() {
	MemoryBus->CPUWrite(0xFF00 | (uint8_t)CurrOperand, A);
}

void Sharp::POP_HL() {
	HL = MemoryBus->CPURead16(SP);
	SP += 2;
}

void Sharp::LD_ADDR_C_A() {
	MemoryBus->CPUWrite(0xFF00 | C, A);
}

void Sharp::PUSH_HL() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, HL);
}

void Sharp::AND_W() {
	And((uint8_t)CurrOperand);
}

void Sharp::RST_20H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0020;
}

void Sharp::ADD_SP_SW() {
	temp3 = CurrOperand & 0x80 ? 0xFF00 | CurrOperand : CurrOperand & 0xFF;
	if (temp3 > (0xFFFF - SP)) {
		SetFlag(c, 1);
	}
	else {
		SetFlag(c, 0);
	}

	temp3 = (SP & 0x0FFF) + (temp3 & 0x0FFF);

	if ((temp3 & 0x1000) == 0x1000) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	SP += (int8_t) CurrOperand;

	SetFlag(z, 0);
	SetFlag(n, 0);
}

void Sharp::JP_HL() {
	PC = HL;
}

void Sharp::LD_ADDR_DW_A() {
	MemoryBus->CPUWrite(CurrOperand, A);
}

void Sharp::XOR_W() {
	Xor((uint8_t) CurrOperand);
}

void Sharp::RST_28H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0028;
}

void Sharp::LDH_A_ADDR_W() {
	A = MemoryBus->CPURead(0xFF00 | (uint8_t) CurrOperand);
}

void Sharp::POP_AF() {
	AF = MemoryBus->CPURead16(SP);
	SP += 2;
}

void Sharp::LD_A_ADDR_C() {
	A = MemoryBus->CPURead(0xFF00 | C);
}

// I've noticed other emulators do not try to emulate the behavior of these DI/EI
// Instructions, where they are delayed by one instruction
// 
// For now, I will try to emulate this behavior but if it affects performance
// too much and ends up being more complicated than not to implement, I will
// instantaneously enable/disable interrupts
void Sharp::DI() {
	PendingIMEChange = 0x10 | 0x02;
}

void Sharp::PUSH_AF() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, AF);
}

void Sharp::OR_W() {
	Or((uint8_t) CurrOperand);
}

void Sharp::RST_30H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0030;
}

void Sharp::LD_HL_SP_PLUS_SW() {
	CurrOperand = CurrOperand & 0x80 ? 0xFF00 | CurrOperand : CurrOperand & 0xFF;
	HL = SP;
	UnsignedAdd16(HL, CurrOperand);
	SetFlag(z, 0);
	SetFlag(n, 0);
}

void Sharp::LD_SP_HL() {
	SP = HL;
}

void Sharp::LD_A_ADDR_DW() {
	A = MemoryBus->CPURead(CurrOperand);
}

void Sharp::EI() {
	PendingIMEChange = 0x90 | 0x02;
}

void Sharp::CP_W() {
	temp = A;
	Subtract(temp, (uint8_t) CurrOperand);
}

void Sharp::RST_38H() {
	SP -= 2;
	MemoryBus->CPUWrite16(SP, PC);
	PC = 0x0038;
}

void Sharp::RLC_B() {
	RotateLeftCircular(B);
}

void Sharp::RLC_C() {
	RotateLeftCircular(C);
}

void Sharp::RLC_D() {
	RotateLeftCircular(D);
}

void Sharp::RLC_E() {
	RotateLeftCircular(E);
}

void Sharp::RLC_H() {
	RotateLeftCircular(H);
}

void Sharp::RLC_L() {
	RotateLeftCircular(L);
}

void Sharp::RLC_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	RotateLeftCircular(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::RLC_A() {
	RotateLeftCircular(A);
}

void Sharp::RRC_B() {
	RotateRightCircular(B);
}

void Sharp::RRC_C() {
	RotateRightCircular(C);
}

void Sharp::RRC_D() {
	RotateRightCircular(D);
}

void Sharp::RRC_E() {
	RotateRightCircular(E);
}

void Sharp::RRC_H() {
	RotateRightCircular(H);
}

void Sharp::RRC_L() {
	RotateRightCircular(L);
}

void Sharp::RRC_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	RotateRightCircular(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::RRC_A() {
	RotateRightCircular(A);
}

void Sharp::RL_B() {
	RotateLeft(B);
}

void Sharp::RL_C() {
	RotateLeft(C);
}

void Sharp::RL_D() {
	RotateLeft(D);
}

void Sharp::RL_E() {
	RotateLeft(E);
}

void Sharp::RL_H() {
	RotateLeft(H);
}

void Sharp::RL_L() {
	RotateLeft(L);
}

void Sharp::RL_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	RotateLeft(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::RL_A() {
	RotateLeft(A);
}

void Sharp::RR_B() {
	RotateRight(B);
}

void Sharp::RR_C() {
	RotateRight(C);
}

void Sharp::RR_D() {
	RotateRight(D);
}

void Sharp::RR_E() {
	RotateRight(E);
}

void Sharp::RR_H() {
	RotateRight(H);
}

void Sharp::RR_L() {
	RotateRight(L);
}

void Sharp::RR_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	RotateRight(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::RR_A() {
	RotateRight(A);
}

void Sharp::SLA_B() {
	ShiftLeft(B);
}

void Sharp::SLA_C() {
	ShiftLeft(C);
}

void Sharp::SLA_D() {
	ShiftLeft(D);
}

void Sharp::SLA_E() {
	ShiftLeft(E);
}

void Sharp::SLA_H() {
	ShiftLeft(H);
}

void Sharp::SLA_L() {
	ShiftLeft(L);
}

void Sharp::SLA_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	ShiftLeft(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::SLA_A() {
	ShiftLeft(A);
}

void Sharp::SRA_B() {
	ShiftRightArithmetic(B);
}

void Sharp::SRA_C() {
	ShiftRightArithmetic(C);
}

void Sharp::SRA_D() {
	ShiftRightArithmetic(D);
}

void Sharp::SRA_E() {
	ShiftRightArithmetic(E);
}

void Sharp::SRA_H() {
	ShiftRightArithmetic(H);
}

void Sharp::SRA_L() {
	ShiftRightArithmetic(L);
}

void Sharp::SRA_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	ShiftRightArithmetic(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::SRA_A() {
	ShiftRightArithmetic(A);
}

void Sharp::SWAP_B() {
	Swap(B);
}

void Sharp::SWAP_C() {
	Swap(C);
}

void Sharp::SWAP_D() {
	Swap(D);
}

void Sharp::SWAP_E() {
	Swap(E);
}

void Sharp::SWAP_H() {
	Swap(H);
}

void Sharp::SWAP_L() {
	Swap(L);
}

void Sharp::SWAP_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	Swap(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::SWAP_A() {
	Swap(A);
}

void Sharp::SRL_B() {
	ShiftRight(B);
}

void Sharp::SRL_C() {
	ShiftRight(C);
}

void Sharp::SRL_D() {
	ShiftRight(D);
}

void Sharp::SRL_E() {
	ShiftRight(E);
}

void Sharp::SRL_H() {
	ShiftRight(H);
}

void Sharp::SRL_L() {
	ShiftRight(L);
}

void Sharp::SRL_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	ShiftRight(temp);
	MemoryBus->CPUWrite(HL, temp);
}

void Sharp::SRL_A() {
	ShiftRight(A);
}

void Sharp::BIT_0_B() {
	Bit(0, B);
}

void Sharp::BIT_0_C() {
	Bit(0, C);
}

void Sharp::BIT_0_D() {
	Bit(0, D);
}

void Sharp::BIT_0_E() {
	Bit(0, E);
}

void Sharp::BIT_0_H() {
	Bit(0, H);
}

void Sharp::BIT_0_L() {
	Bit(0, L);
}

void Sharp::BIT_0_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	Bit(0, temp);
}

void Sharp::BIT_0_A() {
	Bit(0, A);
}

void Sharp::BIT_1_B() {
	Bit(1, B);
}

void Sharp::BIT_1_C() {
	Bit(1, C);
}

void Sharp::BIT_1_D() {
	Bit(1, D);
}

void Sharp::BIT_1_E() {
	Bit(1, E);
}

void Sharp::BIT_1_H() {
	Bit(1, H);
}

void Sharp::BIT_1_L() {
	Bit(1, L);
}

void Sharp::BIT_1_ADDR_HL() {
	temp = MemoryBus->CPURead(HL);
	Bit(1, temp);
}

void Sharp::BIT_1_A() {
	Bit(1, A);
}

Sharp::~Sharp() {
}

void Sharp::Clock() {
	if (CurrCycles == 0) {
		if ((PendingIMEChange & 0x10) && ((PendingIMEChange & 0x0F) > 0)) {
			PendingIMEChange--;
			if ((PendingIMEChange & 0x0F) == 0) {
				InterruptMasterEnable = (PendingIMEChange & 0x80) >> 7;
				PendingIMEChange = 0x00;
			}
		}

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
