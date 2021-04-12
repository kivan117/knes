#pragma once
#include <cstdint>
#include "Mmu.h"
#include "Logger.h"

class Cpu
{
public:
	Cpu(Mmu* _mmu);
	void Tick();
private:
	Mmu* mmu{ nullptr };

	uint64_t totalCycles{ 0 };

	uint8_t opCycles{ 0 };

	struct registers {
		uint8_t   A;	// Accumulator
		uint8_t   X;	// X index
		uint8_t   Y;	// Y index
		uint8_t   P;	// Status register (flags)
		uint8_t   S;	// Stack pointer
		uint16_t PC;	// Program counter
	} Regs;

	struct statusflags {
		bool negative{ false };
		bool overflow{ false };
		bool brk_interrupt{ false };
		bool decimal{ false };
		bool irq_disable{ false };
		bool zero{ false };
		bool carry{ false };
	} Flags;

	enum FLAG_CODE { FLAG_C = 0, FLAG_Z, FLAG_I, FLAG_D, FLAG_B, FLAG_HIGH, FLAG_V, FLAG_N };

	uint8_t Execute(uint8_t op);

	void SetFlag(FLAG_CODE flagCode, uint8_t val);

	uint8_t BaseCyclesLUT[256]{
	//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
		7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, // 0
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 1
		6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, // 2
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4 ,7, 7, // 3
		6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, // 4
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 5
		6, 6, 0, 8, 3, 3 ,5, 5, 4, 2, 2, 2, 5, 4, 6, 6, // 6
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 7
		2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, // 8
		2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, // 9
		2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, // A
		2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, // B
		2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, // C
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // D
		2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, // E
		2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7  // F
	};
};

