#ifndef SHARP_HPP
#define SHARP_HPP

#include <cstdint>
#include <string>
#include <iomanip>
#include <fstream>

class Memory;

// Some Definitions:
// Word (W) = 8 bits / 1 byte
// Double-word (DW) = 16 bits / 2 bytes
// Signed Word (SW) = 8 bits / 1 byte, signed integer

class Sharp {
	// Used to debug CPU operation, but this ability has been removed for now
	std::ofstream Log;
	uint8_t memlog[4];

	uint16_t SP; // Stack pointer
	uint16_t PC; // Program counter (Instruction pointer)

	// Current variables
	uint16_t CurrOperand;
	uint8_t  CurrCycles;
	uint8_t  CurrArgSize;
	uint8_t  Opcode;
	uint8_t  temp;
	uint8_t  temp2;
	uint16_t temp3;
	uint8_t	 InterruptMasterEnable;
	uint8_t  PendingIMEChange;

	// 16-bit registers grouped together using anonymous unions and structs
	union {
		uint16_t AF;
		struct {
			uint8_t F;
			uint8_t A;
		};
	};

	union {
		uint16_t BC;
		struct {
			uint8_t C;
			uint8_t B;
		};
	};

	union {
		uint16_t DE;
		struct {
			uint8_t E;
			uint8_t D;
		};
	};

	union {
		uint16_t HL;
		struct {
			uint8_t L;
			uint8_t H;
		};
	};

	// Enums to help extract flags from the F register
	enum SharpFlags {
		z = (1 << 7),
		n = (1 << 6),
		h = (1 << 5),
		c = (1 << 4)
	};

	void SetFlag(SharpFlags flag, bool set);
	uint8_t GetFlag(SharpFlags flag);

	// Helper functions to reduce repeated code
	void DecrementRegister(uint8_t& reg);
	void IncrementRegister(uint8_t& reg);
	void UnsignedAdd(uint8_t& dest, uint8_t src);
	void UnsignedAddCarry(uint8_t& dest, uint8_t src);
	void UnsignedAdd16(uint16_t& dest, uint16_t src);
	void Subtract(uint8_t& dest, uint8_t src);
	void SubtractWithCarry(uint8_t& dest, uint8_t src);
	void RotateLeftCircular(uint8_t& arg);
	void RotateRightCircular(uint8_t& arg);
	void RotateLeft(uint8_t& arg);
	void RotateRight(uint8_t& arg);
	void ShiftLeft(uint8_t& arg);
	void ShiftRightArithmetic(uint8_t& arg);
	void ShiftRight(uint8_t& arg);
	void Swap(uint8_t& arg);
	void And(uint8_t arg);
	void Xor(uint8_t arg);
	void Or(uint8_t arg);
	void Bit(uint8_t pos, uint8_t arg);
	void SetBit(uint8_t pos, uint8_t& arg);
	void ResetBit(uint8_t pos, uint8_t& arg);
 
	// Needed for the logical CPU to address the memory
	Memory* MemoryBus;

	// SM83 Instruction Set

	// Unknown Opcode
	void UNOP();

	// First Row of Table (0x00 - 0x0F)
	void NOP();
	void LD_BC_DW();
	void LD_ADDR_BC_A();
	void INC_BC();
	void INC_B();
	void DEC_B();
	void LD_B_W();
	void RLCA();
	void LD_ADDR_DW_SP();
	void ADD_HL_BC();
	void LD_A_ADDR_BC();
	void DEC_BC();
	void INC_C();
	void DEC_C();
	void LD_C_W();
	void RRCA();

	// Second Row of Table (0x10 - 0x1F)
	void STOP();
	void LD_DE_DW();
	void LD_ADDR_DE_A();
	void INC_DE();
	void INC_D();
	void DEC_D();
	void LD_D_W();
	void RLA();
	void JR_SW();
	void ADD_HL_DE();
	void LD_A_ADDR_DE();
	void DEC_DE();
	void INC_E();
	void DEC_E();
	void LD_E_W();
	void RRA();

	// Third Row of Table (0x20 - 0x2F)
	void JR_NZ_SW();
	void LD_HL_DW();
	void LD_ADDR_HL_PI_A();
	void INC_HL();
	void INC_H();
	void DEC_H();
	void LD_H_W();
	void DAA();
	void JR_Z_SW();
	void ADD_HL_HL();
	void LD_A_ADDR_HL_PI();
	void DEC_HL();
	void INC_L();
	void DEC_L();
	void LD_L_W();
	void CPL();

	// Fourth Row of Table (0x30 - 0x3F)
	void JR_NC_SW();
	void LD_SP_DW();
	void LD_ADDR_HL_PD_A();
	void INC_SP();
	void INC_ADDR_HL();
	void DEC_ADDR_HL();
	void LD_ADDR_HL_W();
	void SCF();
	void JR_C_SW();
	void ADD_HL_SP();
	void LD_A_ADDR_HL_PD();
	void DEC_SP();
	void INC_A();
	void DEC_A();
	void LD_A_W();
	void CCF();

	// Fifth Row of Table (0x40 - 0x4F)
	void LD_B_B();
	void LD_B_C();
	void LD_B_D();
	void LD_B_E();
	void LD_B_H();
	void LD_B_L();
	void LD_B_ADDR_HL();
	void LD_B_A();
	void LD_C_B();
	void LD_C_C();
	void LD_C_D();
	void LD_C_E();
	void LD_C_H();
	void LD_C_L();
	void LD_C_ADDR_HL();
	void LD_C_A();

	// Sixth Row of Table (0x50 - 0x5F)
	void LD_D_B();
	void LD_D_C();
	void LD_D_D();
	void LD_D_E();
	void LD_D_H();
	void LD_D_L();
	void LD_D_ADDR_HL();
	void LD_D_A();
	void LD_E_B();
	void LD_E_C();
	void LD_E_D();
	void LD_E_E();
	void LD_E_H();
	void LD_E_L();
	void LD_E_ADDR_HL();
	void LD_E_A();

	// Seventh Row of Table (0x60 - 0x6F)
	void LD_H_B();
	void LD_H_C();
	void LD_H_D();
	void LD_H_E();
	void LD_H_H();
	void LD_H_L();
	void LD_H_ADDR_HL();
	void LD_H_A();
	void LD_L_B();
	void LD_L_C();
	void LD_L_D();
	void LD_L_E();
	void LD_L_H();
	void LD_L_L();
	void LD_L_ADDR_HL();
	void LD_L_A();

	// Eighth Row of Table (0x70 - 0x7F)
	void LD_ADDR_HL_B();
	void LD_ADDR_HL_C();
	void LD_ADDR_HL_D();
	void LD_ADDR_HL_E();
	void LD_ADDR_HL_H();
	void LD_ADDR_HL_L();
	void HALT();
	void LD_ADDR_HL_A();
	void LD_A_B();
	void LD_A_C();
	void LD_A_D();
	void LD_A_E();
	void LD_A_H();
	void LD_A_L();
	void LD_A_ADDR_HL();
	void LD_A_A();

	// Ninth Row of Table (0x80 - 0x8F)
	void ADD_A_B();
	void ADD_A_C();
	void ADD_A_D();
	void ADD_A_E();
	void ADD_A_H();
	void ADD_A_L();
	void ADD_A_ADDR_HL();
	void ADD_A_A();
	void ADC_A_B();
	void ADC_A_C();
	void ADC_A_D();
	void ADC_A_E();
	void ADC_A_H();
	void ADC_A_L();
	void ADC_A_ADDR_HL();
	void ADC_A_A();
	
	// Tenth Row of Table (0x90 - 0x9F)
	void SUB_B();
	void SUB_C();
	void SUB_D();
	void SUB_E();
	void SUB_H();
	void SUB_L();
	void SUB_ADDR_HL();
	void SUB_A();
	void SBC_A_B();
	void SBC_A_C();
	void SBC_A_D();
	void SBC_A_E();
	void SBC_A_H();
	void SBC_A_L();
	void SBC_A_ADDR_HL();
	void SBC_A_A();

	// Eleventh Row of Table (0xA0 - 0xAF)
	void AND_B();
	void AND_C();
	void AND_D();
	void AND_E();
	void AND_H();
	void AND_L();
	void AND_ADDR_HL();
	void AND_A();
	void XOR_B();
	void XOR_C();
	void XOR_D();
	void XOR_E();
	void XOR_H();
	void XOR_L();
	void XOR_ADDR_HL();
	void XOR_A();

	// Twelfth Row of Table (0xB0 - 0xBF)
	void OR_B();
	void OR_C();
	void OR_D();
	void OR_E();
	void OR_H();
	void OR_L();
	void OR_ADDR_HL();
	void OR_A();
	void CP_B();
	void CP_C();
	void CP_D();
	void CP_E();
	void CP_H();
	void CP_L();
	void CP_ADDR_HL();
	void CP_A();

	// Thirteenth Row of Table (0xC0 - 0xCF)
	void RET_NZ();
	void POP_BC();
	void JP_NZ_ADDR_DW();
	void JP_ADDR_DW();
	void CALL_NZ_ADDR_DW();
	void PUSH_BC();
	void ADD_A_W();
	void RST_00H();
	void RET_Z();
	void RET();
	void JP_Z_ADDR_DW();
	void PREFIX_CB();
	void CALL_Z_ADDR_DW();
	void CALL_ADDR_DW();
	void ADC_A_W();
	void RST_08H();

	// Fourteenth Row of Table (0xD0 - 0xDF)
	void RET_NC();
	void POP_DE();
	void JP_NC_ADDR_DW();
	// Unknown Opcode
	void CALL_NC_ADDR_DW();
	void PUSH_DE();
	void SUB_W();
	void RST_10H();
	void RET_C();
	void RETI();
	void JP_C_ADDR_DW();
	// Unknown Opcode
	void CALL_C_ADDR_DW();
	// Unknown Opcode
	void SBC_A_W();
	void RST_18H();

	// Fifteenth Row of Table (0xE0 - 0xEF)
	void LDH_ADDR_W_A();
	void POP_HL();
	void LD_ADDR_C_A();
	// Unknown Opcode
	// Unknown Opcode
	void PUSH_HL();
	void AND_W();
	void RST_20H();
	void ADD_SP_SW();
	void JP_HL();
	void LD_ADDR_DW_A();
	// Unknown Opcode
	// Unknown Opcode
	void XOR_W();
	void RST_28H();

	// Sixteenth Row of Table (0xF0 - 0xFF)
	void LDH_A_ADDR_W();
	void POP_AF();
	void LD_A_ADDR_C();
	void DI();
	// Unknown Opcode
	void PUSH_AF();
	void OR_W();
	void RST_30H();
	void LD_HL_SP_PLUS_SW();
	void LD_SP_HL();
	void LD_A_ADDR_DW();
	void EI();
	void CP_W();
	// Unknown Opcode
	// Unknown Opcode
	void RST_38H();

	// 0xCB Instructions
	// First row of table (0x00 - 0x0F)
	void RLC_B();
	void RLC_C();
	void RLC_D();
	void RLC_E();
	void RLC_H();
	void RLC_L();
	void RLC_ADDR_HL();
	void RLC_A();
	void RRC_B();
	void RRC_C();
	void RRC_D();
	void RRC_E();
	void RRC_H();
	void RRC_L();
	void RRC_ADDR_HL();
	void RRC_A();

	// Second row of table (0x10 - 0x1F)
	void RL_B();
	void RL_C();
	void RL_D();
	void RL_E();
	void RL_H();
	void RL_L();
	void RL_ADDR_HL();
	void RL_A();
	void RR_B();
	void RR_C();
	void RR_D();
	void RR_E();
	void RR_H();
	void RR_L();
	void RR_ADDR_HL();
	void RR_A();

	// Third row of table (0x20 - 0x2F)
	void SLA_B();
	void SLA_C();
	void SLA_D();
	void SLA_E();
	void SLA_H();
	void SLA_L();
	void SLA_ADDR_HL();
	void SLA_A();
	void SRA_B();
	void SRA_C();
	void SRA_D();
	void SRA_E();
	void SRA_H();
	void SRA_L();
	void SRA_ADDR_HL();
	void SRA_A();

	// Fourth row of table (0x30 - 0x3F)
	void SWAP_B();
	void SWAP_C();
	void SWAP_D();
	void SWAP_E();
	void SWAP_H();
	void SWAP_L();
	void SWAP_ADDR_HL();
	void SWAP_A();
	void SRL_B();
	void SRL_C();
	void SRL_D();
	void SRL_E();
	void SRL_H();
	void SRL_L();
	void SRL_ADDR_HL();
	void SRL_A();

	// Fifth row of table (0x40 - 0x4F)
	void BIT_0_B();
	void BIT_0_C();
	void BIT_0_D();
	void BIT_0_E();
	void BIT_0_H();
	void BIT_0_L();
	void BIT_0_ADDR_HL();
	void BIT_0_A();
	void BIT_1_B();
	void BIT_1_C();
	void BIT_1_D();
	void BIT_1_E();
	void BIT_1_H();
	void BIT_1_L();
	void BIT_1_ADDR_HL();
	void BIT_1_A();

	// Sixth row of table (0x50 - 0x5F)
	void BIT_2_B();
	void BIT_2_C();
	void BIT_2_D();
	void BIT_2_E();
	void BIT_2_H();
	void BIT_2_L();
	void BIT_2_ADDR_HL();
	void BIT_2_A();
	void BIT_3_B();
	void BIT_3_C();
	void BIT_3_D();
	void BIT_3_E();
	void BIT_3_H();
	void BIT_3_L();
	void BIT_3_ADDR_HL();
	void BIT_3_A();

	// Seventh row of table (0x60 - 0x6F)
	void BIT_4_B();
	void BIT_4_C();
	void BIT_4_D();
	void BIT_4_E();
	void BIT_4_H();
	void BIT_4_L();
	void BIT_4_ADDR_HL();
	void BIT_4_A();
	void BIT_5_B();
	void BIT_5_C();
	void BIT_5_D();
	void BIT_5_E();
	void BIT_5_H();
	void BIT_5_L();
	void BIT_5_ADDR_HL();
	void BIT_5_A();

	// Eighth row of table (0x70 - 0x7F)
	void BIT_6_B();
	void BIT_6_C();
	void BIT_6_D();
	void BIT_6_E();
	void BIT_6_H();
	void BIT_6_L();
	void BIT_6_ADDR_HL();
	void BIT_6_A();
	void BIT_7_B();
	void BIT_7_C();
	void BIT_7_D();
	void BIT_7_E();
	void BIT_7_H();
	void BIT_7_L();
	void BIT_7_ADDR_HL();
	void BIT_7_A();

	// Ninth row of table (0x80 - 0x8F)
	void RES_0_B();
	void RES_0_C();
	void RES_0_D();
	void RES_0_E();
	void RES_0_H();
	void RES_0_L();
	void RES_0_ADDR_HL();
	void RES_0_A();
	void RES_1_B();
	void RES_1_C();
	void RES_1_D();
	void RES_1_E();
	void RES_1_H();
	void RES_1_L();
	void RES_1_ADDR_HL();
	void RES_1_A();

	// Tenth row of table (0x90 - 0x9F)
	void RES_2_B();
	void RES_2_C();
	void RES_2_D();
	void RES_2_E();
	void RES_2_H();
	void RES_2_L();
	void RES_2_ADDR_HL();
	void RES_2_A();
	void RES_3_B();
	void RES_3_C();
	void RES_3_D();
	void RES_3_E();
	void RES_3_H();
	void RES_3_L();
	void RES_3_ADDR_HL();
	void RES_3_A();

	// Eleventh row of table (0xA0 - 0xAF)
	void RES_4_B();
	void RES_4_C();
	void RES_4_D();
	void RES_4_E();
	void RES_4_H();
	void RES_4_L();
	void RES_4_ADDR_HL();
	void RES_4_A();
	void RES_5_B();
	void RES_5_C();
	void RES_5_D();
	void RES_5_E();
	void RES_5_H();
	void RES_5_L();
	void RES_5_ADDR_HL();
	void RES_5_A();

	// Twelfth row of table (0xB0 - 0xBF)
	void RES_6_B();
	void RES_6_C();
	void RES_6_D();
	void RES_6_E();
	void RES_6_H();
	void RES_6_L();
	void RES_6_ADDR_HL();
	void RES_6_A();
	void RES_7_B();
	void RES_7_C();
	void RES_7_D();
	void RES_7_E();
	void RES_7_H();
	void RES_7_L();
	void RES_7_ADDR_HL();
	void RES_7_A();

	// Thirteenth row of table (0xC0 - 0xCF)
	void SET_0_B();
	void SET_0_C();
	void SET_0_D();
	void SET_0_E();
	void SET_0_H();
	void SET_0_L();
	void SET_0_ADDR_HL();
	void SET_0_A();
	void SET_1_B();
	void SET_1_C();
	void SET_1_D();
	void SET_1_E();
	void SET_1_H();
	void SET_1_L();
	void SET_1_ADDR_HL();
	void SET_1_A();

	// Fourteenth row of table (0xC0 - 0xCF)
	void SET_2_B();
	void SET_2_C();
	void SET_2_D();
	void SET_2_E();
	void SET_2_H();
	void SET_2_L();
	void SET_2_ADDR_HL();
	void SET_2_A();
	void SET_3_B();
	void SET_3_C();
	void SET_3_D();
	void SET_3_E();
	void SET_3_H();
	void SET_3_L();
	void SET_3_ADDR_HL();
	void SET_3_A();

	// Fifteenth row of table (0xA0 - 0xAF)
	void SET_4_B();
	void SET_4_C();
	void SET_4_D();
	void SET_4_E();
	void SET_4_H();
	void SET_4_L();
	void SET_4_ADDR_HL();
	void SET_4_A();
	void SET_5_B();
	void SET_5_C();
	void SET_5_D();
	void SET_5_E();
	void SET_5_H();
	void SET_5_L();
	void SET_5_ADDR_HL();
	void SET_5_A();

	// Sixteenth row of table (0xB0 - 0xBF)
	void SET_6_B();
	void SET_6_C();
	void SET_6_D();
	void SET_6_E();
	void SET_6_H();
	void SET_6_L();
	void SET_6_ADDR_HL();
	void SET_6_A();
	void SET_7_B();
	void SET_7_C();
	void SET_7_D();
	void SET_7_E();
	void SET_7_H();
	void SET_7_L();
	void SET_7_ADDR_HL();
	void SET_7_A();

	struct SharpInstr {
		uint8_t ArgSize; // Can be 0 words, 1 word, or 2 words
		uint8_t Cycles; // Number of cycles the instructions take
		void (Sharp::*Instruction)(void); // Pointer to a function
	};

	// Holder variable for the current instruction
	SharpInstr DecodedInstr;
	const struct SharpInstr SHARPINSTRS[256] = {
		{0,  4, &Sharp::NOP            }, {2, 12, &Sharp::LD_BC_DW    }, {0,  8, &Sharp::LD_ADDR_BC_A   }, {0,  8, &Sharp::INC_BC	   }, 
		{0,  4, &Sharp::INC_B          }, {0,  4, &Sharp::DEC_B       }, {1,  8, &Sharp::LD_B_W         }, {0,  4, &Sharp::RLCA		   }, 
		{2, 20, &Sharp::LD_ADDR_DW_SP  }, {0,  8, &Sharp::ADD_HL_BC   }, {0,  8, &Sharp::LD_A_ADDR_BC   }, {0,  8, &Sharp::DEC_BC	   },
		{0,  4, &Sharp::INC_C          }, {0,  4, &Sharp::DEC_C       }, {1,  8, &Sharp::LD_C_W         }, {0,  4, &Sharp::RRCA		   },
									  							    										 
		{1,  4, &Sharp::STOP           }, {2, 12, &Sharp::LD_DE_DW    }, {0, 12, &Sharp::LD_ADDR_DE_A   }, {0,  8, &Sharp::INC_DE	   },
		{0,  4, &Sharp::INC_D          }, {0,  4, &Sharp::DEC_D       }, {1,  8, &Sharp::LD_D_W         }, {0,  4, &Sharp::RLA		   },
		{1, 12, &Sharp::JR_SW          }, {0,  8, &Sharp::ADD_HL_DE   }, {0,  8, &Sharp::LD_A_ADDR_DE   }, {0,  8, &Sharp::DEC_DE	   },
		{0,  4, &Sharp::INC_E          }, {0,  4, &Sharp::DEC_E       }, {1,  8, &Sharp::LD_E_W         }, {0,  4, &Sharp::RRA 		   },
									  							    										 
		{1,  8, &Sharp::JR_NZ_SW       }, {2, 12, &Sharp::LD_HL_DW    }, {0,  8, &Sharp::LD_ADDR_HL_PI_A}, {0,  8, &Sharp::INC_HL	   },
		{0,  4, &Sharp::INC_H          }, {0,  4, &Sharp::DEC_H       }, {1,  8, &Sharp::LD_H_W         }, {0,  4, &Sharp::DAA		   },
		{1,  8, &Sharp::JR_Z_SW        }, {0,  8, &Sharp::ADD_HL_HL   }, {0,  8, &Sharp::LD_A_ADDR_HL_PI}, {0,  8, &Sharp::DEC_HL	   },
		{0,  4, &Sharp::INC_L          }, {0,  4, &Sharp::DEC_L       }, {1,  8, &Sharp::LD_L_W         }, {0,  4, &Sharp::CPL	 	   },
									  																		 
		{1,  8, &Sharp::JR_NC_SW       }, {2, 12, &Sharp::LD_SP_DW    }, {0,  8, &Sharp::LD_ADDR_HL_PD_A}, {0,  8, &Sharp::INC_SP      },
		{0, 12, &Sharp::INC_ADDR_HL    }, {0, 12, &Sharp::DEC_ADDR_HL }, {1, 12, &Sharp::LD_ADDR_HL_W   }, {0,  4, &Sharp::SCF         },
		{1,  8, &Sharp::JR_C_SW        }, {0,  8, &Sharp::ADD_HL_SP   }, {0,  8, &Sharp::LD_A_ADDR_HL_PD}, {0,  8, &Sharp::DEC_SP      },
		{0,  4, &Sharp::INC_A          }, {0,  4, &Sharp::DEC_A       }, {1,  8, &Sharp::LD_A_W         }, {0,  4, &Sharp::CCF         },
									  							    										 
		{0,  4, &Sharp::LD_B_B         }, {0,  4, &Sharp::LD_B_C      }, {0,  4, &Sharp::LD_B_D         }, {0,  4, &Sharp::LD_B_E      },
		{0,  4, &Sharp::LD_B_H         }, {0,  4, &Sharp::LD_B_L      }, {0,  8, &Sharp::LD_B_ADDR_HL   }, {0,  4, &Sharp::LD_B_A      },
		{0,  4, &Sharp::LD_C_B         }, {0,  4, &Sharp::LD_C_C      }, {0,  4, &Sharp::LD_C_D         }, {0,  4, &Sharp::LD_C_E      },
		{0,  4, &Sharp::LD_C_H         }, {0,  4, &Sharp::LD_C_L      }, {0,  8, &Sharp::LD_C_ADDR_HL   }, {0,  4, &Sharp::LD_C_A      },
									  							    										 
		{0,  4, &Sharp::LD_D_B         }, {0,  4, &Sharp::LD_D_C      }, {0,  4, &Sharp::LD_D_D         }, {0,  4, &Sharp::LD_D_E      },
		{0,  4, &Sharp::LD_D_H         }, {0,  4, &Sharp::LD_D_L      }, {0,  8, &Sharp::LD_D_ADDR_HL   }, {0,  4, &Sharp::LD_D_A      },
		{0,  4, &Sharp::LD_E_B         }, {0,  4, &Sharp::LD_E_C      }, {0,  4, &Sharp::LD_E_D         }, {0,  4, &Sharp::LD_E_E      },
		{0,  4, &Sharp::LD_E_H         }, {0,  4, &Sharp::LD_E_L      }, {0,  8, &Sharp::LD_E_ADDR_HL   }, {0,  4, &Sharp::LD_E_A      },
									  							    										 
		{0,  4, &Sharp::LD_H_B         }, {0,  4, &Sharp::LD_H_C      }, {0,  4, &Sharp::LD_H_D         }, {0,  4, &Sharp::LD_H_E      },
		{0,  4, &Sharp::LD_H_H         }, {0,  4, &Sharp::LD_H_L      }, {0,  8, &Sharp::LD_H_ADDR_HL   }, {0,  4, &Sharp::LD_H_A      },
		{0,  4, &Sharp::LD_L_B         }, {0,  4, &Sharp::LD_L_C      }, {0,  4, &Sharp::LD_L_D         }, {0,  4, &Sharp::LD_L_E      },
		{0,  4, &Sharp::LD_L_H         }, {0,  4, &Sharp::LD_L_L      }, {0,  8, &Sharp::LD_L_ADDR_HL   }, {0,  4, &Sharp::LD_L_A      },
									  																		 
		{0,  4, &Sharp::LD_ADDR_HL_B   }, {0,  4, &Sharp::LD_ADDR_HL_C}, {0,  4, &Sharp::LD_ADDR_HL_D   }, {0,  4, &Sharp::LD_ADDR_HL_E},
		{0,  4, &Sharp::LD_ADDR_HL_H   }, {0,  4, &Sharp::LD_ADDR_HL_L}, {0,  8, &Sharp::HALT           }, {0,  4, &Sharp::LD_ADDR_HL_A},
		{0,  4, &Sharp::LD_A_B         }, {0,  4, &Sharp::LD_A_C      }, {0,  4, &Sharp::LD_A_D         }, {0,  4, &Sharp::LD_A_E      },
		{0,  4, &Sharp::LD_A_H         }, {0,  4, &Sharp::LD_A_L      }, {0,  8, &Sharp::LD_A_ADDR_HL   }, {0,  4, &Sharp::LD_A_A      },
									  																		 
		{0,  4, &Sharp::ADD_A_B        }, {0,  4, &Sharp::ADD_A_C     }, {0,  4, &Sharp::ADD_A_D        }, {0,  4, &Sharp::ADD_A_E     },
		{0,  4, &Sharp::ADD_A_H        }, {0,  4, &Sharp::ADD_A_L     }, {0,  8, &Sharp::ADD_A_ADDR_HL  }, {0,  4, &Sharp::ADD_A_A     },
		{0,  4, &Sharp::ADC_A_B        }, {0,  4, &Sharp::ADC_A_C     }, {0,  4, &Sharp::ADC_A_D        }, {0,  4, &Sharp::ADC_A_E     },
		{0,  4, &Sharp::ADC_A_H        }, {0,  4, &Sharp::ADC_A_L     }, {0,  8, &Sharp::ADC_A_ADDR_HL  }, {0,  4, &Sharp::ADC_A_A     },
									  																		 
		{0,  4, &Sharp::SUB_B          }, {0,  4, &Sharp::SUB_C       }, {0,  4, &Sharp::SUB_D          }, {0,  4, &Sharp::SUB_E       },
		{0,  4, &Sharp::SUB_H          }, {0,  4, &Sharp::SUB_L       }, {0,  8, &Sharp::SUB_ADDR_HL    }, {0,  4, &Sharp::SUB_A       },
		{0,  4, &Sharp::SBC_A_B        }, {0,  4, &Sharp::SBC_A_C     }, {0,  4, &Sharp::SBC_A_D        }, {0,  4, &Sharp::SBC_A_E     },
		{0,  4, &Sharp::SBC_A_H        }, {0,  4, &Sharp::SBC_A_L     }, {0,  8, &Sharp::SBC_A_ADDR_HL  }, {0,  4, &Sharp::SBC_A_A     },
									  																		 
		{0,  4, &Sharp::AND_B          }, {0,  4, &Sharp::AND_C       }, {0,  4, &Sharp::AND_D          }, {0,  4, &Sharp::AND_E       },
		{0,  4, &Sharp::AND_H          }, {0,  4, &Sharp::AND_L       }, {0,  8, &Sharp::AND_ADDR_HL    }, {0,  4, &Sharp::AND_A       },
		{0,  4, &Sharp::XOR_B          }, {0,  4, &Sharp::XOR_C       }, {0,  4, &Sharp::XOR_D          }, {0,  4, &Sharp::XOR_E       },
		{0,  4, &Sharp::XOR_H          }, {0,  4, &Sharp::XOR_L       }, {0,  8, &Sharp::XOR_ADDR_HL    }, {0,  4, &Sharp::XOR_A       },
									  																		 
		{0,  4, &Sharp::OR_B           }, {0,  4, &Sharp::OR_C        }, {0,  4, &Sharp::OR_D           }, {0,  4, &Sharp::OR_E        },
		{0,  4, &Sharp::OR_H           }, {0,  4, &Sharp::OR_L        }, {0,  8, &Sharp::OR_ADDR_HL     }, {0,  4, &Sharp::OR_A        },
		{0,  4, &Sharp::CP_B           }, {0,  4, &Sharp::CP_C        }, {0,  4, &Sharp::CP_D           }, {0,  4, &Sharp::CP_E        },
		{0,  4, &Sharp::CP_H           }, {0,  4, &Sharp::CP_L        }, {0,  8, &Sharp::CP_ADDR_HL     }, {0,  4, &Sharp::CP_A        },
																											 
		{0,  8, &Sharp::RET_NZ		   }, {0, 12, &Sharp::POP_BC	  }, {2, 12, &Sharp::JP_NZ_ADDR_DW  }, {2, 16, &Sharp::JP_ADDR_DW  },
		{2, 12, &Sharp::CALL_NZ_ADDR_DW}, {0, 16, &Sharp::PUSH_BC     }, {1,  8, &Sharp::ADD_A_W        }, {0, 16, &Sharp::RST_00H     },
		{0,  8, &Sharp::RET_Z		   }, {0, 16, &Sharp::RET         }, {2, 12, &Sharp::JP_Z_ADDR_DW   }, {0,  0, &Sharp::PREFIX_CB   },
		{2, 12, &Sharp::CALL_Z_ADDR_DW }, {2, 24, &Sharp::CALL_ADDR_DW}, {1,  8, &Sharp::ADC_A_W        }, {0, 16, &Sharp::RST_08H     },

		{0,  8, &Sharp::RET_NC		   }, {0, 12, &Sharp::POP_DE	  }, {2, 12, &Sharp::JP_NC_ADDR_DW  }, {0,  4, &Sharp::UNOP		   },
		{2, 12, &Sharp::CALL_NC_ADDR_DW}, {0, 16, &Sharp::PUSH_DE	  }, {1,  8, &Sharp::SUB_W			}, {0, 16, &Sharp::RST_10H	   },
		{0,  8, &Sharp::RET_C		   }, {0, 16, &Sharp::RETI		  }, {2, 12, &Sharp::JP_C_ADDR_DW	}, {0,  4, &Sharp::UNOP		   },
		{2, 12, &Sharp::CALL_C_ADDR_DW }, {0,  4, &Sharp::UNOP		  }, {1,  8, &Sharp::SBC_A_W		}, {0, 16, &Sharp::RST_18H	   },
				 
		{1, 12, &Sharp::LDH_ADDR_W_A    }, {0, 12, &Sharp::POP_HL	  }, {0,  8, &Sharp::LD_ADDR_C_A	}, {0,  4, &Sharp::UNOP		   },
		{0,  4, &Sharp::UNOP			}, {0, 16, &Sharp::PUSH_HL	  }, {1,  8, &Sharp::AND_W			}, {0, 16, &Sharp::RST_20H	   },
		{1, 16, &Sharp::ADD_SP_SW		}, {0,  4, &Sharp::JP_HL	  }, {2, 16, &Sharp::LD_ADDR_DW_A	}, {0,  4, &Sharp::UNOP		   },
		{0,  4, &Sharp::UNOP			}, {0,  4, &Sharp::UNOP		  }, {1,  8, &Sharp::XOR_W			}, {0, 16, &Sharp::RST_28H	   },

		{1, 12, &Sharp::LDH_A_ADDR_W	}, {0, 12, &Sharp::POP_AF     }, {0,  8, &Sharp::LD_A_ADDR_C	}, {0,  4, &Sharp::DI		   },
		{0,  4, &Sharp::UNOP			}, {0, 16, &Sharp::PUSH_AF	  }, {1,  8, &Sharp::OR_W			}, {0, 16, &Sharp::RST_30H	   },
		{1, 12, &Sharp::LD_HL_SP_PLUS_SW}, {0,  8, &Sharp::LD_SP_HL	  }, {2, 16, &Sharp::LD_A_ADDR_DW	}, {0,  4, &Sharp::EI		   },
		{0,  4, &Sharp::UNOP			}, {0,  4, &Sharp::UNOP		  }, {1,  8, &Sharp::CP_W			}, {0, 16, &Sharp::RST_38H	   }
	};	  		

	const struct SharpInstr SHARPINSTRS_CB[256] = {
		{0, 8, &Sharp::RLC_B   }, {0, 8, &Sharp::RLC_C  }, {0,  8, &Sharp::RLC_D		}, {0, 8, &Sharp::RLC_E },
		{0, 8, &Sharp::RLC_H   }, {0, 8, &Sharp::RLC_L  }, {0, 16, &Sharp::RLC_ADDR_HL  }, {0, 8, &Sharp::RLC_A },
		{0, 8, &Sharp::RRC_B   }, {0, 8, &Sharp::RRC_C  }, {0,  8, &Sharp::RRC_D		}, {0, 8, &Sharp::RRC_E },
		{0, 8, &Sharp::RRC_H   }, {0, 8, &Sharp::RRC_L  }, {0, 16, &Sharp::RRC_ADDR_HL  }, {0, 8, &Sharp::RRC_A },

		{0, 8, &Sharp::RL_B    }, {0, 8, &Sharp::RL_C   }, {0,  8, &Sharp::RL_D		    }, {0, 8, &Sharp::RL_E  },
		{0, 8, &Sharp::RL_H    }, {0, 8, &Sharp::RL_L   }, {0, 16, &Sharp::RL_ADDR_HL   }, {0, 8, &Sharp::RL_A  },
		{0, 8, &Sharp::RR_B    }, {0, 8, &Sharp::RR_C   }, {0,  8, &Sharp::RR_D		    }, {0, 8, &Sharp::RR_E  },
		{0, 8, &Sharp::RR_H    }, {0, 8, &Sharp::RR_L   }, {0, 16, &Sharp::RR_ADDR_HL   }, {0, 8, &Sharp::RR_A  },
							   						    
		{0, 8, &Sharp::SLA_B   }, {0, 8, &Sharp::SLA_C  }, {0,  8, &Sharp::SLA_D		}, {0, 8, &Sharp::SLA_E },
		{0, 8, &Sharp::SLA_H   }, {0, 8, &Sharp::SLA_L  }, {0, 16, &Sharp::SLA_ADDR_HL  }, {0, 8, &Sharp::SLA_A },
		{0, 8, &Sharp::SRA_B   }, {0, 8, &Sharp::SRA_C  }, {0,  8, &Sharp::SRA_D		}, {0, 8, &Sharp::SRA_E },
		{0, 8, &Sharp::SRA_H   }, {0, 8, &Sharp::SRA_L  }, {0, 16, &Sharp::SRA_ADDR_HL  }, {0, 8, &Sharp::SRA_A },
													    
		{0, 8, &Sharp::SWAP_B  }, {0, 8, &Sharp::SWAP_C }, {0,  8, &Sharp::SWAP_D		}, {0, 8, &Sharp::SWAP_E},
		{0, 8, &Sharp::SWAP_H  }, {0, 8, &Sharp::SWAP_L }, {0, 16, &Sharp::SWAP_ADDR_HL }, {0, 8, &Sharp::SWAP_A},
		{0, 8, &Sharp::SRL_B   }, {0, 8, &Sharp::SRL_C  }, {0,  8, &Sharp::SRL_D		}, {0, 8, &Sharp::SRL_E },
		{0, 8, &Sharp::SRL_H   }, {0, 8, &Sharp::SRL_L  }, {0, 16, &Sharp::SRL_ADDR_HL  }, {0, 8, &Sharp::SRL_A },

		{0, 8, &Sharp::BIT_0_B }, {0, 8, &Sharp::BIT_0_C}, {0,  8, &Sharp::BIT_0_D		}, {0, 8, &Sharp::BIT_0_E},
		{0, 8, &Sharp::BIT_0_H }, {0, 8, &Sharp::BIT_0_L}, {0, 12, &Sharp::BIT_0_ADDR_HL}, {0, 8, &Sharp::BIT_0_A},
		{0, 8, &Sharp::BIT_1_B }, {0, 8, &Sharp::BIT_1_C}, {0,  8, &Sharp::BIT_1_D		}, {0, 8, &Sharp::BIT_1_E},
		{0, 8, &Sharp::BIT_1_H }, {0, 8, &Sharp::BIT_1_L}, {0, 12, &Sharp::BIT_1_ADDR_HL}, {0, 8, &Sharp::BIT_1_A},

		{0, 8, &Sharp::BIT_2_B }, {0, 8, &Sharp::BIT_2_C}, {0,  8, &Sharp::BIT_2_D		}, {0, 8, &Sharp::BIT_2_E},
		{0, 8, &Sharp::BIT_2_H }, {0, 8, &Sharp::BIT_2_L}, {0, 12, &Sharp::BIT_2_ADDR_HL}, {0, 8, &Sharp::BIT_2_A},
		{0, 8, &Sharp::BIT_3_B }, {0, 8, &Sharp::BIT_3_C}, {0,  8, &Sharp::BIT_3_D		}, {0, 8, &Sharp::BIT_3_E},
		{0, 8, &Sharp::BIT_3_H }, {0, 8, &Sharp::BIT_3_L}, {0, 12, &Sharp::BIT_3_ADDR_HL}, {0, 8, &Sharp::BIT_3_A},

		{0, 8, &Sharp::BIT_4_B }, {0, 8, &Sharp::BIT_4_C}, {0,  8, &Sharp::BIT_4_D		}, {0, 8, &Sharp::BIT_4_E},
		{0, 8, &Sharp::BIT_4_H }, {0, 8, &Sharp::BIT_4_L}, {0, 12, &Sharp::BIT_4_ADDR_HL}, {0, 8, &Sharp::BIT_4_A},
		{0, 8, &Sharp::BIT_5_B }, {0, 8, &Sharp::BIT_5_C}, {0,  8, &Sharp::BIT_5_D		}, {0, 8, &Sharp::BIT_5_E},
		{0, 8, &Sharp::BIT_5_H }, {0, 8, &Sharp::BIT_5_L}, {0, 12, &Sharp::BIT_5_ADDR_HL}, {0, 8, &Sharp::BIT_5_A},

		{0, 8, &Sharp::BIT_6_B }, {0, 8, &Sharp::BIT_6_C}, {0,  8, &Sharp::BIT_6_D		}, {0, 8, &Sharp::BIT_6_E},
		{0, 8, &Sharp::BIT_6_H }, {0, 8, &Sharp::BIT_6_L}, {0, 12, &Sharp::BIT_6_ADDR_HL}, {0, 8, &Sharp::BIT_6_A},
		{0, 8, &Sharp::BIT_7_B }, {0, 8, &Sharp::BIT_7_C}, {0,  8, &Sharp::BIT_7_D		}, {0, 8, &Sharp::BIT_7_E},
		{0, 8, &Sharp::BIT_7_H }, {0, 8, &Sharp::BIT_7_L}, {0, 12, &Sharp::BIT_7_ADDR_HL}, {0, 8, &Sharp::BIT_7_A},

		{0, 8, &Sharp::RES_0_B }, {0, 8, &Sharp::RES_0_C}, {0,  8, &Sharp::RES_0_D		}, {0, 8, &Sharp::RES_0_E},
		{0, 8, &Sharp::RES_0_H }, {0, 8, &Sharp::RES_0_L}, {0, 16, &Sharp::RES_0_ADDR_HL}, {0, 8, &Sharp::RES_0_A},
		{0, 8, &Sharp::RES_1_B }, {0, 8, &Sharp::RES_1_C}, {0,  8, &Sharp::RES_1_D		}, {0, 8, &Sharp::RES_1_E},
		{0, 8, &Sharp::RES_1_H }, {0, 8, &Sharp::RES_1_L}, {0, 16, &Sharp::RES_1_ADDR_HL}, {0, 8, &Sharp::RES_1_A},

		{0, 8, &Sharp::RES_2_B }, {0, 8, &Sharp::RES_2_C}, {0,  8, &Sharp::RES_2_D		}, {0, 8, &Sharp::RES_2_E},
		{0, 8, &Sharp::RES_2_H }, {0, 8, &Sharp::RES_2_L}, {0, 16, &Sharp::RES_2_ADDR_HL}, {0, 8, &Sharp::RES_2_A},
		{0, 8, &Sharp::RES_3_B }, {0, 8, &Sharp::RES_3_C}, {0,  8, &Sharp::RES_3_D		}, {0, 8, &Sharp::RES_3_E},
		{0, 8, &Sharp::RES_3_H }, {0, 8, &Sharp::RES_3_L}, {0, 16, &Sharp::RES_3_ADDR_HL}, {0, 8, &Sharp::RES_3_A},

		{0, 8, &Sharp::RES_4_B }, {0, 8, &Sharp::RES_4_C}, {0,  8, &Sharp::RES_4_D		}, {0, 8, &Sharp::RES_4_E},
		{0, 8, &Sharp::RES_4_H }, {0, 8, &Sharp::RES_4_L}, {0, 16, &Sharp::RES_4_ADDR_HL}, {0, 8, &Sharp::RES_4_A},
		{0, 8, &Sharp::RES_5_B }, {0, 8, &Sharp::RES_5_C}, {0,  8, &Sharp::RES_5_D		}, {0, 8, &Sharp::RES_5_E},
		{0, 8, &Sharp::RES_5_H }, {0, 8, &Sharp::RES_5_L}, {0, 16, &Sharp::RES_5_ADDR_HL}, {0, 8, &Sharp::RES_5_A},

		{0, 8,& Sharp::RES_6_B }, {0, 8, &Sharp::RES_6_C}, {0,  8, &Sharp::RES_6_D		}, {0, 8, &Sharp::RES_6_E},
		{0, 8, &Sharp::RES_6_H }, {0, 8, &Sharp::RES_6_L}, {0, 16, &Sharp::RES_6_ADDR_HL}, {0, 8, &Sharp::RES_6_A},
		{0, 8, &Sharp::RES_7_B }, {0, 8, &Sharp::RES_7_C}, {0,  8, &Sharp::RES_7_D		}, {0, 8, &Sharp::RES_7_E},
		{0, 8, &Sharp::RES_7_H }, {0, 8, &Sharp::RES_7_L}, {0, 16, &Sharp::RES_7_ADDR_HL}, {0, 8, &Sharp::RES_7_A},

		{0, 8, &Sharp::SET_0_B }, {0, 8, &Sharp::SET_0_C}, {0,  8, &Sharp::SET_0_D		}, {0, 8, &Sharp::SET_0_E},
		{0, 8, &Sharp::SET_0_H }, {0, 8, &Sharp::SET_0_L}, {0, 16, &Sharp::SET_0_ADDR_HL}, {0, 8, &Sharp::SET_0_A},
		{0, 8, &Sharp::SET_1_B }, {0, 8, &Sharp::SET_1_C}, {0,  8, &Sharp::SET_1_D		}, {0, 8, &Sharp::SET_1_E},
		{0, 8, &Sharp::SET_1_H }, {0, 8, &Sharp::SET_1_L}, {0, 16, &Sharp::SET_1_ADDR_HL}, {0, 8, &Sharp::SET_1_A},

		{0, 8, &Sharp::SET_2_B }, {0, 8, &Sharp::SET_2_C}, {0,  8, &Sharp::SET_2_D		}, {0, 8, &Sharp::SET_2_E},
		{0, 8, &Sharp::SET_2_H }, {0, 8, &Sharp::SET_2_L}, {0, 16, &Sharp::SET_2_ADDR_HL}, {0, 8, &Sharp::SET_2_A},
		{0, 8, &Sharp::SET_3_B }, {0, 8, &Sharp::SET_3_C}, {0,  8, &Sharp::SET_3_D		}, {0, 8, &Sharp::SET_3_E},
		{0, 8, &Sharp::SET_3_H }, {0, 8, &Sharp::SET_3_L}, {0, 16, &Sharp::SET_3_ADDR_HL}, {0, 8, &Sharp::SET_3_A},

		{0, 8, &Sharp::SET_4_B }, {0, 8, &Sharp::SET_4_C}, {0,  8, &Sharp::SET_4_D		}, {0, 8, &Sharp::SET_4_E},
		{0, 8, &Sharp::SET_4_H }, {0, 8, &Sharp::SET_4_L}, {0, 16, &Sharp::SET_4_ADDR_HL}, {0, 8, &Sharp::SET_4_A},
		{0, 8, &Sharp::SET_5_B }, {0, 8, &Sharp::SET_5_C}, {0,  8, &Sharp::SET_5_D		}, {0, 8, &Sharp::SET_5_E},
		{0, 8, &Sharp::SET_5_H }, {0, 8, &Sharp::SET_5_L}, {0, 16, &Sharp::SET_5_ADDR_HL}, {0, 8, &Sharp::SET_5_A},

		{0, 8, &Sharp::SET_6_B }, {0, 8, &Sharp::SET_6_C}, {0,  8, &Sharp::SET_6_D		}, {0, 8, &Sharp::SET_6_E},
		{0, 8, &Sharp::SET_6_H }, {0, 8, &Sharp::SET_6_L}, {0, 16, &Sharp::SET_6_ADDR_HL}, {0, 8, &Sharp::SET_6_A},
		{0, 8, &Sharp::SET_7_B }, {0, 8, &Sharp::SET_7_C}, {0,  8, &Sharp::SET_7_D		}, {0, 8, &Sharp::SET_7_E},
		{0, 8, &Sharp::SET_7_H }, {0, 8, &Sharp::SET_7_L}, {0, 16, &Sharp::SET_7_ADDR_HL}, {0, 8, &Sharp::SET_7_A}
	};											  

public:
	bool Suspended;

	Sharp(Memory* _MemoryBus);
	~Sharp();

	void Clock();
};

#endif
