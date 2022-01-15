#ifndef SHARP_HPP
#define SHARP_HPP

#include <cstdint>
#include <string>

class Memory;

class Sharp {
public:
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

	// Flag Register & helper funcs for the flag register
	// Takes up one byte in the CPU
	uint8_t Flags;

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

	// Needed for the logical CPU to address the memory
	Memory* MemoryBus;

	// SM83 Instruction Set
	void NOP();
	void LD_BC_NN();
	void LD_ADDR_BC_A();
	void INC_BC();
	void INC_B();
	void DEC_B();
	void LD_B_N();
	void RCLA();
	void LD_ADDR_NN_SP();
	void ADD_HL_BC();
	void LD_A_ADDR_BC();
	void DEC_BC();
	void INC_C();
	void LD_C_N();

	struct SharpInstr {
		uint8_t ArgSize; // Can be 0 words, 1 word, or 2 words
		uint8_t Cycles; // Number of cycles the instructions take
		void (Sharp::*Instruction)(void); // Pointer to a function
	};

	// Holder variable for the current instruction
	SharpInstr DecodedInstr;

	const struct SharpInstr SHARPINSTRS[14] = {
		{0,  4, &Sharp::NOP      }, {2, 12, &Sharp::LD_BC_NN    }, {0,  8, &Sharp::LD_ADDR_BC_A }, 
		{0,  8, &Sharp::INC_BC   }, {0,  4, &Sharp::INC_B       }, {0,  4, &Sharp::DEC_B        },
		{1,  8, &Sharp::LD_B_N   }, {0,  4, &Sharp::RCLA        }, {2, 20, &Sharp::LD_ADDR_NN_SP},
		{0,  8, &Sharp::ADD_HL_BC}, {0,  8, &Sharp::LD_A_ADDR_BC}, {0,  8, &Sharp::DEC_BC       },
		{0,  4, &Sharp::INC_C    }, {1,  8, &Sharp::LD_C_N      }
	};

public:
	Sharp(Memory* _MemoryBus);
	~Sharp();

	void Clock();
};

#endif
