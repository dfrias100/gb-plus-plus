#include "Z80.hpp"

Z80::Z80(Memory* _MemoryBus)
{
	MemoryBus = _MemoryBus;
}

void Z80::NOP()
{
}

Z80::~Z80()
{
}

void Z80::Clock()
{
	if (CurrCycles == 0) {
		Opcode = MemoryBus->CPURead(PC++);
		CurrArgSize = Z80INSTRS[Opcode].ArgSize;

		switch (CurrArgSize) {
			case 1:
				CurrOperand = (uint16_t) MemoryBus->CPURead(PC);
				break;
			case 2:
				CurrOperand = MemoryBus->CPURead16(PC);
				break;
		}

		(this->*Z80INSTRS[Opcode].Instruction)();
	}
}
