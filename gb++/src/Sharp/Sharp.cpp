#include "Sharp.hpp"
#include "../Memory/Memory.hpp"
#include <iostream>
#include <iomanip>

Sharp::Sharp(Memory* _MemoryBus) {
	MemoryBus = _MemoryBus;
	Suspended = false;
	HaltBug = false;

	// These are the register values when the PC is at
	// $0100 after the boot ROM is unmapped from memory
	PC = 0x0100;
	AF = 0x01B0;
	BC = 0x0013;
	DE = 0x00D8;
	HL = 0x014D;
	SP = 0xFFFE;
	InterruptMasterEnable = 0x01;

	//Log.open("execlog.txt", std::ios::trunc | std::ios::binary);
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
	if ((reg & 0xF) == 0) {
		SetFlag(h, 1);
	} else {
		SetFlag(h, 0);
	}

	reg--;

	if (reg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 1);
}

void Sharp::IncrementRegister(uint8_t& reg) {
	if ((reg & 0xF) == 0xF) {
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

	temp2 = (dest & 0xF) + (src & 0xF) + temp;

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
	if (src > dest) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	if ((src & 0xF) > (dest & 0xF)) {
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

	if (src > dest || (temp && (dest - src) == 0x0)) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	if ((src & 0xF) > (dest & 0xF) || (temp && ((src & 0xF) - (dest & 0xF)) == 0)) {
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
	temp2 = GetFlag(c);
	SetFlag(c, (arg & 0x80) >> 7);

	arg <<= 1;
	arg += temp2;

	if (arg == 0) {
		SetFlag(z, 1);
	} else {
		SetFlag(z, 0);
	}

	SetFlag(n, 0);
	SetFlag(h, 0);
}

void Sharp::RotateRight(uint8_t& arg) {
	temp2 = GetFlag(c);
	SetFlag(c, arg & 0x1);

	arg >>= 1;
	arg += (temp2 << 7);

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
	temp2 = arg & 0xF;
	arg &= 0xF0;
	arg >>= 4;
	arg |= (temp2 << 4);


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
	SetFlag(n, 0);
	SetFlag(h, 1);
}

void Sharp::SetBit(uint8_t pos, uint8_t& arg) {
	arg |= 0x01 << pos;
}

void Sharp::ResetBit(uint8_t pos, uint8_t& arg) {
	arg = arg & ~(0x01 << pos);
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
	MemoryBus->WriteWord(BC, A);
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
	MemoryBus->WriteDoubleWord(CurrOperand, SP);
}

void Sharp::ADD_HL_BC() {
	UnsignedAdd16(HL, BC);
}

void Sharp::LD_A_ADDR_BC() {
	A = MemoryBus->ReadWord(BC);
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
	// I will ignore this instruction for now
}

void Sharp::LD_DE_DW() {
	DE = CurrOperand;
}

void Sharp::LD_ADDR_DE_A() {
	MemoryBus->WriteWord(DE, A);
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
	SetFlag(z, 0);
}

void Sharp::JR_SW() {
	PC = PC + (int8_t) (CurrOperand);
}

void Sharp::ADD_HL_DE() {
	UnsignedAdd16(HL, DE);
}

void Sharp::LD_A_ADDR_DE() {
	A = MemoryBus->ReadWord(DE);
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
	SetFlag(z, 0);
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
	MemoryBus->WriteWord(HL++, A);
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

	A += GetFlag(n) ? -temp : temp;

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
	A = MemoryBus->ReadWord(HL++);
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
	MemoryBus->WriteWord(HL--, A);
}

void Sharp::INC_SP() {
	SP++;
}

void Sharp::INC_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	IncrementRegister(temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::DEC_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	DecrementRegister(temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::LD_ADDR_HL_W() {
	MemoryBus->WriteWord(HL, (uint8_t) CurrOperand);
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
	A = MemoryBus->ReadWord(HL--);
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
	B = MemoryBus->ReadWord(HL);
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
	C = MemoryBus->ReadWord(HL);
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
	D = MemoryBus->ReadWord(HL);
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
	E = MemoryBus->ReadWord(HL);
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
	H = MemoryBus->ReadWord(HL);
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
	L = MemoryBus->ReadWord(HL);
}

void Sharp::LD_L_A() {
	L = A;
}

void Sharp::LD_ADDR_HL_B() {
	MemoryBus->WriteWord(HL, B);
}

void Sharp::LD_ADDR_HL_C() {
	MemoryBus->WriteWord(HL, C);
}

void Sharp::LD_ADDR_HL_D() {
	MemoryBus->WriteWord(HL, D);
}

void Sharp::LD_ADDR_HL_E() {
	MemoryBus->WriteWord(HL, E);
}

void Sharp::LD_ADDR_HL_H() {
	MemoryBus->WriteWord(HL, H);
}

void Sharp::LD_ADDR_HL_L() {
	MemoryBus->WriteWord(HL, L);
}

void Sharp::HALT() {
	// We only want to enter halt mode if the IME is enabled or we do not have a pending interrupt
	// Otherwise, we trigger the halt bug
	if (InterruptMasterEnable) {
		Suspended = true;
	} else {
		if ((MemoryBus->InterruptEnableRegister & *(MemoryBus->InterruptFlags) & 0x1F) == 0) {
			Suspended = true;
		} else {
			HaltBug = true;
		}
	}
}

void Sharp::LD_ADDR_HL_A() {
	MemoryBus->WriteWord(HL, A);
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
	A = MemoryBus->ReadWord(HL);
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
	temp = MemoryBus->ReadWord(HL);
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
	temp = MemoryBus->ReadWord(HL);
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
	temp = MemoryBus->ReadWord(HL);
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
	temp = MemoryBus->ReadWord(HL);
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
	temp = MemoryBus->ReadWord(HL);
	And(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	Xor(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	Or(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp2 = MemoryBus->ReadWord(HL);
	Subtract(temp, temp2);
}

void Sharp::CP_A() {
	temp = A;
	Subtract(temp, A);
}

void Sharp::RET_NZ() {
	if (!GetFlag(z)) {
		PC = MemoryBus->ReadDoubleWord(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::POP_BC() {
	BC = MemoryBus->ReadDoubleWord(SP);
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
		MemoryBus->WriteDoubleWord(SP, PC);
		PC = CurrOperand;
		CurrCycles += 12;
	}
}

void Sharp::PUSH_BC() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, BC);
}

void Sharp::ADD_A_W() {
	UnsignedAdd(A, (uint8_t) CurrOperand);
}

void Sharp::RST_00H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0000;
}

void Sharp::RET_Z() {
	if (GetFlag(z)) {
		PC = MemoryBus->ReadDoubleWord(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::RET() {
	PC = MemoryBus->ReadDoubleWord(SP);
	SP += 2;
}

void Sharp::JP_Z_ADDR_DW() {
	if (GetFlag(z)) {
		PC = CurrOperand;
		CurrCycles += 4;
	}
}

void Sharp::PREFIX_CB() {
	Opcode = MemoryBus->ReadWord(PC++);
	DecodedInstr = SHARPINSTRS_CB[Opcode];

	CurrCycles = DecodedInstr.Cycles;
	CurrArgSize = DecodedInstr.ArgSize;

	(this->*DecodedInstr.Instruction)();
}

void Sharp::CALL_Z_ADDR_DW() {
	if (GetFlag(z)) {
		SP -= 2;
		MemoryBus->WriteDoubleWord(SP, PC);
		PC = CurrOperand;
		CurrCycles += 12;
	}
}

void Sharp::CALL_ADDR_DW() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = CurrOperand;
}

void Sharp::ADC_A_W() {
	UnsignedAddCarry(A, (uint8_t) CurrOperand);
}

void Sharp::RST_08H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0008;
}

void Sharp::RET_NC() {
	if (!GetFlag(c)) {
		PC = MemoryBus->ReadDoubleWord(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::POP_DE() {
	DE = MemoryBus->ReadDoubleWord(SP);
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
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = CurrOperand;
	CurrCycles += 12;
}
}

void Sharp::PUSH_DE() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, DE);
}

void Sharp::SUB_W() {
	Subtract(A, (uint8_t)CurrOperand);
}

void Sharp::RST_10H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0010;
}

void Sharp::RET_C() {
	if (GetFlag(c)) {
		PC = MemoryBus->ReadDoubleWord(SP);
		SP += 2;
		CurrCycles += 12;
	}
}

void Sharp::RETI() {
	PC = MemoryBus->ReadDoubleWord(SP);
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
		MemoryBus->WriteDoubleWord(SP, PC);
		PC = CurrOperand;
		CurrCycles += 12;
	}
}

void Sharp::SBC_A_W() {
	SubtractWithCarry(A, (uint8_t) CurrOperand);
}

void Sharp::RST_18H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0018;
}

void Sharp::LDH_ADDR_W_A() {
	MemoryBus->WriteWord(0xFF00 | (uint8_t)CurrOperand, A);
}

void Sharp::POP_HL() {
	HL = MemoryBus->ReadDoubleWord(SP);
	SP += 2;
}

void Sharp::LD_ADDR_C_A() {
	MemoryBus->WriteWord(0xFF00 | C, A);
}

void Sharp::PUSH_HL() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, HL);
}

void Sharp::AND_W() {
	And((uint8_t)CurrOperand);
}

void Sharp::RST_20H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0020;
}

void Sharp::ADD_SP_SW() {
	temp3 = CurrOperand & 0x80 ? 0xFF00 | CurrOperand : CurrOperand & 0xFF;
	if ((temp3 & 0xFF) > (0xFF - (SP & 0xFF))) {
		SetFlag(c, 1);
	} else {
		SetFlag(c, 0);
	}

	temp3 = (SP & 0xF) + (temp3 & 0xF);

	if ((temp3 & 0x10) == 0x10) {
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
	MemoryBus->WriteWord(CurrOperand, A);
}

void Sharp::XOR_W() {
	Xor((uint8_t) CurrOperand);
}

void Sharp::RST_28H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0028;
}

void Sharp::LDH_A_ADDR_W() {
	A = MemoryBus->ReadWord(0xFF00 | (uint8_t) CurrOperand);
}

void Sharp::POP_AF() {
	AF = MemoryBus->ReadDoubleWord(SP) & 0xFFF0;
	SP += 2;
}

void Sharp::LD_A_ADDR_C() {
	A = MemoryBus->ReadWord(0xFF00 | C);
}

// Blargg's ROMs seem to conclude the behavior of these instructions are O.K.
void Sharp::DI() {
	PendingIMEChange = 0x10 | 0x02;
}

void Sharp::PUSH_AF() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, AF);
}

void Sharp::OR_W() {
	Or((uint8_t) CurrOperand);
}

void Sharp::RST_30H() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x0030;
}

void Sharp::LD_HL_SP_PLUS_SW() {
	CurrOperand = CurrOperand & 0x80 ? 0xFF00 | CurrOperand : CurrOperand & 0xFF;
	temp = SP;
	UnsignedAdd(temp, CurrOperand);
	HL = SP + (int8_t) CurrOperand;
	SetFlag(z, 0);
	SetFlag(n, 0);
}

void Sharp::LD_SP_HL() {
	SP = HL;
}

void Sharp::LD_A_ADDR_DW() {
	A = MemoryBus->ReadWord(CurrOperand);
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
	MemoryBus->WriteDoubleWord(SP, PC);
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
	temp = MemoryBus->ReadWord(HL);
	RotateLeftCircular(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	RotateRightCircular(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	RotateLeft(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	RotateRight(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	ShiftLeft(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	ShiftRightArithmetic(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	Swap(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
	ShiftRight(temp);
	MemoryBus->WriteWord(HL, temp);
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
	temp = MemoryBus->ReadWord(HL);
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
	temp = MemoryBus->ReadWord(HL);
	Bit(1, temp);
}

void Sharp::BIT_1_A() {
	Bit(1, A);
}

void Sharp::BIT_2_B() {
	Bit(2, B);
}

void Sharp::BIT_2_C() {
	Bit(2, C);
}

void Sharp::BIT_2_D() {
	Bit(2, D);
}

void Sharp::BIT_2_E() {
	Bit(2, E);
}

void Sharp::BIT_2_H() {
	Bit(2, H);
}

void Sharp::BIT_2_L() {
	Bit(2, L);
}

void Sharp::BIT_2_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	Bit(2, temp);
}

void Sharp::BIT_2_A() {
	Bit(2, A);
}

void Sharp::BIT_3_B() {
	Bit(3, B);
}

void Sharp::BIT_3_C() {
	Bit(3, C);
}

void Sharp::BIT_3_D() {
	Bit(3, D);
}

void Sharp::BIT_3_E() {
	Bit(3, E);
}

void Sharp::BIT_3_H() {
	Bit(3, H);
}

void Sharp::BIT_3_L() {
	Bit(3, L);
}

void Sharp::BIT_3_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	Bit(3, temp);
}

void Sharp::BIT_3_A() {
	Bit(3, A);
}

void Sharp::BIT_4_B() {
	Bit(4, B);
}

void Sharp::BIT_4_C() {
	Bit(4, C);
}

void Sharp::BIT_4_D() {
	Bit(4, D);
}

void Sharp::BIT_4_E() {
	Bit(4, E);
}

void Sharp::BIT_4_H() {
	Bit(4, H);
}

void Sharp::BIT_4_L() {
	Bit(4, L);
}

void Sharp::BIT_4_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	Bit(4, temp);
}

void Sharp::BIT_4_A() {
	Bit(4, A);
}

void Sharp::BIT_5_B() {
	Bit(5, B);
}

void Sharp::BIT_5_C() {
	Bit(5, C);
}

void Sharp::BIT_5_D() {
	Bit(5, D);
}

void Sharp::BIT_5_E() {
	Bit(5, E);
}

void Sharp::BIT_5_H() {
	Bit(5, H);
}

void Sharp::BIT_5_L() {
	Bit(5, L);
}

void Sharp::BIT_5_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	Bit(5, temp);
}

void Sharp::BIT_5_A() {
	Bit(5, A);
}

void Sharp::BIT_6_B() {
	Bit(6, B);
}

void Sharp::BIT_6_C() {
	Bit(6, C);
}

void Sharp::BIT_6_D() {
	Bit(6, D);
}

void Sharp::BIT_6_E() {
	Bit(6, E);
}

void Sharp::BIT_6_H() {
	Bit(6, H);
}

void Sharp::BIT_6_L() {
	Bit(6, L);
}

void Sharp::BIT_6_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	Bit(6, temp);
}

void Sharp::BIT_6_A() {
	Bit(6, A);
}

void Sharp::BIT_7_B() {
	Bit(7, B);
}

void Sharp::BIT_7_C() {
	Bit(7, C);
}

void Sharp::BIT_7_D() {
	Bit(7, D);
}

void Sharp::BIT_7_E() {
	Bit(7, E);
}

void Sharp::BIT_7_H() {
	Bit(7, H);
}

void Sharp::BIT_7_L() {
	Bit(7, L);
}

void Sharp::BIT_7_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	Bit(7, temp);
}

void Sharp::BIT_7_A() {
	Bit(7, A);
}

void Sharp::RES_0_B() {
	ResetBit(0, B);
}

void Sharp::RES_0_C() {
	ResetBit(0, C);
}

void Sharp::RES_0_D() {
	ResetBit(0, D);
}

void Sharp::RES_0_E() {
	ResetBit(0, E);
}

void Sharp::RES_0_H() {
	ResetBit(0, H);
}

void Sharp::RES_0_L() {
	ResetBit(0, L);
}

void Sharp::RES_0_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(0, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_0_A() {
	ResetBit(0, A);
}

void Sharp::RES_1_B() {
	ResetBit(1, B);
}

void Sharp::RES_1_C() {
	ResetBit(1, C);
}

void Sharp::RES_1_D() {
	ResetBit(1, D);
}

void Sharp::RES_1_E() {
	ResetBit(1, E);
}

void Sharp::RES_1_H() {
	ResetBit(1, H);
}

void Sharp::RES_1_L() {
	ResetBit(1, L);
}

void Sharp::RES_1_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(1, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_1_A() {
	ResetBit(1, A);
}

void Sharp::RES_2_B() {
	ResetBit(2, B);
}

void Sharp::RES_2_C() {
	ResetBit(2, C);
}

void Sharp::RES_2_D() { 
	ResetBit(2, D);
}

void Sharp::RES_2_E() {
	ResetBit(2, E);
}

void Sharp::RES_2_H() {
	ResetBit(2, H);
}

void Sharp::RES_2_L() {
	ResetBit(2, L);
}

void Sharp::RES_2_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(2, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_2_A() {
	ResetBit(2, A);
}

void Sharp::RES_3_B() {
	ResetBit(3, B);
}

void Sharp::RES_3_C() {
	ResetBit(3, C);
}

void Sharp::RES_3_D() {
	ResetBit(3, D);
}

void Sharp::RES_3_E() {
	ResetBit(3, E);
}

void Sharp::RES_3_H() {
	ResetBit(3, H);
}

void Sharp::RES_3_L() {
	ResetBit(3, L);
}

void Sharp::RES_3_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(3, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_3_A() {
	ResetBit(3, A);
}

void Sharp::RES_4_B() {
	ResetBit(4, B);
}

void Sharp::RES_4_C() {
	ResetBit(4, C);
}

void Sharp::RES_4_D() {
	ResetBit(4, D);
}

void Sharp::RES_4_E() {
	ResetBit(4, E);
}

void Sharp::RES_4_H() {
	ResetBit(4, H);
}

void Sharp::RES_4_L() {
	ResetBit(4, L);
}

void Sharp::RES_4_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(4, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_4_A() {
	ResetBit(4, A);
}

void Sharp::RES_5_B() {
	ResetBit(5, B);
}

void Sharp::RES_5_C() {
	ResetBit(5, C);
}

void Sharp::RES_5_D() {
	ResetBit(5, D);
}

void Sharp::RES_5_E() {
	ResetBit(5, E);
}

void Sharp::RES_5_H() {
	ResetBit(5, H);
}

void Sharp::RES_5_L() {
	ResetBit(5, L);
}

void Sharp::RES_5_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(5, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_5_A() {
	ResetBit(5, A);
}

void Sharp::RES_6_B() {
	ResetBit(6, B);
}

void Sharp::RES_6_C() {
	ResetBit(6, C);
}

void Sharp::RES_6_D() {
	ResetBit(6, D);
}

void Sharp::RES_6_E() {
	ResetBit(6, E);
}

void Sharp::RES_6_H() {
	ResetBit(6, H);
}

void Sharp::RES_6_L() {
	ResetBit(6, L);
}

void Sharp::RES_6_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(6, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_6_A() {
	ResetBit(6, A);
}

void Sharp::RES_7_B() {
	ResetBit(7, B);
}

void Sharp::RES_7_C() {
	ResetBit(7, C);
}

void Sharp::RES_7_D() {
	ResetBit(7, D);
}

void Sharp::RES_7_E() {
	ResetBit(7, E);
}

void Sharp::RES_7_H() {
	ResetBit(7, H);
}

void Sharp::RES_7_L() {
	ResetBit(7, L);
}

void Sharp::RES_7_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	ResetBit(7, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::RES_7_A() {
	ResetBit(7, A);
}

void Sharp::SET_0_B() {
	SetBit(0, B);
}

void Sharp::SET_0_C() {
	SetBit(0, C);
}

void Sharp::SET_0_D() {
	SetBit(0, D);
}

void Sharp::SET_0_E() {
	SetBit(0, E);
}

void Sharp::SET_0_H() {
	SetBit(0, H);
}

void Sharp::SET_0_L() {
	SetBit(0, L);
}

void Sharp::SET_0_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(0, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_0_A() {
	SetBit(0, A);
}

void Sharp::SET_1_B() {
	SetBit(1, B);
}

void Sharp::SET_1_C() {
	SetBit(1, C);
}

void Sharp::SET_1_D() {
	SetBit(1, D);
}

void Sharp::SET_1_E() {
	SetBit(1, E);
}

void Sharp::SET_1_H() {
	SetBit(1, H);
}

void Sharp::SET_1_L() {
	SetBit(1, L);
}

void Sharp::SET_1_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(1, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_1_A() {
	SetBit(1, A);
}

void Sharp::SET_2_B() {
	SetBit(2, B);
}

void Sharp::SET_2_C() {
	SetBit(2, C);
}

void Sharp::SET_2_D() {
	SetBit(2, D);
}

void Sharp::SET_2_E() {
	SetBit(2, E);
}

void Sharp::SET_2_H() {
	SetBit(2, H);
}

void Sharp::SET_2_L() {
	SetBit(2, L);
}

void Sharp::SET_2_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(2, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_2_A() {
	SetBit(2, A);
}

void Sharp::SET_3_B() {
	SetBit(3, B);
}

void Sharp::SET_3_C() {
	SetBit(3, C);
}

void Sharp::SET_3_D() {
	SetBit(3, D);
}

void Sharp::SET_3_E() {
	SetBit(3, E);
}

void Sharp::SET_3_H() {
	SetBit(3, H);
}

void Sharp::SET_3_L() {
	SetBit(3, L);
}

void Sharp::SET_3_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(3, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_3_A() {
	SetBit(3, A);
}

void Sharp::SET_4_B() {
	SetBit(4, B);
}

void Sharp::SET_4_C() {
	SetBit(4, C);
}

void Sharp::SET_4_D() {
	SetBit(4, D);
}

void Sharp::SET_4_E() {
	SetBit(4, E);
}

void Sharp::SET_4_H() {
	SetBit(4, H);
}

void Sharp::SET_4_L() {
	SetBit(4, L);
}

void Sharp::SET_4_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(4, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_4_A() {
	SetBit(4, A);
}

void Sharp::SET_5_B() {
	SetBit(5, B);
}

void Sharp::SET_5_C() {
	SetBit(5, C);
}

void Sharp::SET_5_D() {
	SetBit(5, D);
}

void Sharp::SET_5_E() {
	SetBit(5, E);
}

void Sharp::SET_5_H() {
	SetBit(5, H);
}

void Sharp::SET_5_L() {
	SetBit(5, L);
}

void Sharp::SET_5_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(5, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_5_A() {
	SetBit(5, A);
}

void Sharp::SET_6_B() {
	SetBit(6, B);
}

void Sharp::SET_6_C() {
	SetBit(6, C);
}

void Sharp::SET_6_D() {
	SetBit(6, D);
}

void Sharp::SET_6_E() {
	SetBit(6, E);
}

void Sharp::SET_6_H() {
	SetBit(6, H);
}

void Sharp::SET_6_L() {
	SetBit(6, L);
}

void Sharp::SET_6_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(6, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_6_A() {
	SetBit(6, A);
}

void Sharp::SET_7_B() {
	SetBit(7, B);
}

void Sharp::SET_7_C() {
	SetBit(7, C);
}

void Sharp::SET_7_D() {
	SetBit(7, D);
}

void Sharp::SET_7_E() {
	SetBit(7, E);
}

void Sharp::SET_7_H() {
	SetBit(7, H);
}

void Sharp::SET_7_L() {
	SetBit(7, L);
}

void Sharp::SET_7_ADDR_HL() {
	temp = MemoryBus->ReadWord(HL);
	SetBit(7, temp);
	MemoryBus->WriteWord(HL, temp);
}

void Sharp::SET_7_A() {
	SetBit(7, A);
}

void Sharp::INT_40() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x40;
	CurrCycles += 12;
}

void Sharp::INT_48() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x48;
	CurrCycles += 12;
}

void Sharp::INT_50() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x50;
	CurrCycles += 12;
}

void Sharp::INT_58() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x58;
	CurrCycles += 12;
}

void Sharp::INT_60() {
	SP -= 2;
	MemoryBus->WriteDoubleWord(SP, PC);
	PC = 0x60;
	CurrCycles += 12;
}

Sharp::~Sharp() {
	Log.close();
}

// This is kind of a mess, but looking past all the pointer dereferencing
// we exit halt (if any) and service the ISR based on the bits set 
void Sharp::InterruptHandler() {
	if (MemoryBus->InterruptEnableRegister & *(MemoryBus->InterruptFlags) & 0x1F) {
		Suspended = false;
	}

	if (InterruptMasterEnable && MemoryBus->InterruptEnableRegister && *(MemoryBus->InterruptFlags)) {
		temp = MemoryBus->InterruptEnableRegister & *(MemoryBus->InterruptFlags) & 0x1F;

		if (temp & VBLANK) {
			*(MemoryBus->InterruptFlags) &= ~VBLANK;
			INT_40();
		}

		if (temp & LCDSTAT) {
			*(MemoryBus->InterruptFlags) &= ~LCDSTAT;
			INT_48();
		}

		if (temp & TIMER) {
			*(MemoryBus->InterruptFlags) &= ~TIMER;
			INT_50();
		}

		if (temp & SERIAL) {
			*(MemoryBus->InterruptFlags) &= ~SERIAL;
			INT_58();
		}

		if (temp & JOYPAD) {
			*(MemoryBus->InterruptFlags) &= ~JOYPAD;
			INT_60();
		}
	}
}

void Sharp::Clock() {
	if (!Suspended) {
		if (CurrCycles == 0) {
			if (MemoryBus->DummyDMACopyFlag) {
				MemoryBus->DummyDMACopyFlag = false;
				MemoryBus->DMACopyFlag = true;
			}

			switch(HaltBug) {
				case false:
					Opcode = MemoryBus->ReadWord(PC++);
					break;
				case true:
					Opcode = MemoryBus->ReadWord(PC);
					HaltBug = false;
					break;
			}
			DecodedInstr = SHARPINSTRS[Opcode];

			CurrCycles = DecodedInstr.Cycles;
			CurrArgSize = DecodedInstr.ArgSize;

			switch (CurrArgSize) {
				case 1:
					CurrOperand = (uint16_t) MemoryBus->ReadWord(PC++);
					break;
				case 2:
					CurrOperand = MemoryBus->ReadDoubleWord(PC);
					PC += 2;
					break;
			}

			(this->*DecodedInstr.Instruction)();

			if ((PendingIMEChange & 0x10) && ((PendingIMEChange & 0x0F) > 0)) {
				PendingIMEChange--;
				if ((PendingIMEChange & 0x0F) == 0) {
					InterruptMasterEnable = (PendingIMEChange & 0x80) >> 7;
					PendingIMEChange = 0x00;
				}
			}
		}
		CurrCycles--;
	}
}

/*/
	Should the emulator report to the CPU that it will be loading the
	boot ROM, these values should be zero iniialized; we will let the
	code from the boot ROM set the registers when as it reaches PC = $0100
*/
void Sharp::SetupBootRom() {
	PC = 0x0000;
	AF = 0x0000;
	BC = 0x0000;
	DE = 0x0000;
	HL = 0x0000;
	SP = 0xFFFE;
}
