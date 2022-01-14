#ifndef Z80_HPP
#define Z80_HPP

#include <cstdint>
#include <string>

#include "../Memory/Memory.hpp"

class Z80 {
public:
	uint16_t SP; // Stack pointer
	uint16_t PC; // Program counter (Instruction pointer)

	// Current variables
	uint16_t CurrOperand;
	uint8_t CurrCycles;
	uint8_t CurrArgSize;
	uint8_t Opcode;

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
		uint16_t DE;
		struct {
			uint8_t L;
			uint8_t H;
		};
	};

	// Flag Register, takes up one byte
	uint8_t Flags;

	enum Z80Flags {
		Z = (1 << 7),
		N = (1 << 6),
		H = (1 << 5),
		C = (1 << 4)
	};

	Memory* MemoryBus;


	// Instruction Set

	void NOP();

	struct Z80Instr {
		uint8_t ArgSize; // Can be 0 words, half-word, or 1 word
		uint8_t cycles; // Number of cycles the instructions take
		void (Z80::* Instruction)(void); // Pointer to a function
	};

	Z80Instr Z80INSTRS[1] = {
		{0, 4, &Z80::NOP}
	};

public:
	Z80(Memory* _MemoryBus);
	~Z80();

	void Clock();
};

#endif
