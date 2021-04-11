#pragma once
#include <cstdint>
#include "Mmu.h"

class Cpu
{
public:
	void Tick();
private:
	struct registers {
		uint8_t   A;	// Accumulator
		uint8_t   X;	// X index
		uint8_t   Y;	// Y index
		uint8_t   P;	// Status register (flags)
		uint8_t   S;	// Stack pointer
		uint16_t PC;	// Program counter
	} Regs;
};

