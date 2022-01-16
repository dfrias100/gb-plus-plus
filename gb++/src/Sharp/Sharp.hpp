#ifndef SHARP_HPP
#define SHARP_HPP

#include <cstdint>
#include <string>

class Memory;

// Some Definitions:
// Word (W) = 8 bits / 1 byte
// Double-word (DW) = 16 bits / 2 bytes
// Signed Word (SW) = 8 bits / 1 byte, signed integer

class Sharp {
	uint16_t SP; // Stack pointer
	uint16_t PC; // Program counter (Instruction pointer)

	// Current variables
	uint16_t CurrOperand;
	uint8_t  CurrCycles;
	uint8_t  CurrArgSize;
	uint8_t  Opcode;
	uint8_t  temp;

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
	void UnsignedAdd16(uint16_t& dest, uint16_t src);
	void RotateLeftCircular(uint8_t& arg);
	void RotateRightCircular(uint8_t& arg);
	void RotateLeft(uint8_t& arg);
	void RotateRight(uint8_t& arg);
 
	// Needed for the logical CPU to address the memory
	Memory* MemoryBus;

	// SM83 Instruction Set

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

	struct SharpInstr {
		uint8_t ArgSize; // Can be 0 words, 1 word, or 2 words
		uint8_t Cycles; // Number of cycles the instructions take
		void (Sharp::*Instruction)(void); // Pointer to a function
	};

	// Holder variable for the current instruction
	SharpInstr DecodedInstr;
	const struct SharpInstr SHARPINSTRS[48] = {
		{0,  4, &Sharp::NOP          }, {2, 12, &Sharp::LD_BC_DW }, {0,  8, &Sharp::LD_ADDR_BC_A   }, {0, 8, &Sharp::INC_BC}, 
		{0,  4, &Sharp::INC_B        }, {0,  4, &Sharp::DEC_B    }, {1,  8, &Sharp::LD_B_W         }, {0, 4, &Sharp::RLCA  }, 
		{2, 20, &Sharp::LD_ADDR_DW_SP}, {0,  8, &Sharp::ADD_HL_BC}, {0,  8, &Sharp::LD_A_ADDR_BC   }, {0, 8, &Sharp::DEC_BC},
		{0,  4, &Sharp::INC_C        }, {0,  4, &Sharp::DEC_C    }, {1,  8, &Sharp::LD_C_W         }, {0, 4, &Sharp::RRCA  },

		{1,  4, &Sharp::STOP         }, {2, 12, &Sharp::LD_DE_DW }, {0, 12, &Sharp::LD_ADDR_BC_A   }, {0, 8, &Sharp::INC_DE},
		{0,  4, &Sharp::INC_D        }, {0,  4, &Sharp::DEC_D    }, {1,  8, &Sharp::LD_D_W         }, {0, 4, &Sharp::RLA   },
		{1, 12, &Sharp::JR_SW        }, {0,  8, &Sharp::ADD_HL_DE}, {0,  8, &Sharp::LD_A_ADDR_DE   }, {0, 8, &Sharp::DEC_DE},
		{0,  4, &Sharp::INC_E        }, {0,  4, &Sharp::DEC_E    }, {1,  8, &Sharp::LD_E_W         }, {0, 4, &Sharp::RRA   },

		{1,  8, &Sharp::JR_NZ_SW     }, {2, 12, &Sharp::LD_HL_DW }, {0,  8, &Sharp::LD_ADDR_HL_PI_A}, {0, 8, &Sharp::INC_HL},
		{0,  4, &Sharp::INC_H        }, {0,  4, &Sharp::DEC_H    }, {1,  8, &Sharp::LD_H_W         }, {0, 4, &Sharp::DAA   },
		{1,  8, &Sharp::JR_Z_SW      }, {0,  8, &Sharp::ADD_HL_HL}, {0,  8, &Sharp::LD_A_ADDR_HL_PI}, {0, 8, &Sharp::DEC_HL},
		{0,  4, &Sharp::INC_L        }, {0,  4, &Sharp::DEC_L    }, {1,  8, &Sharp::LD_L_W         }, {0, 4, &Sharp::CPL   }
	};

public:
	bool Suspended;

	Sharp(Memory* _MemoryBus);
	~Sharp();

	void Clock();
};

#endif
