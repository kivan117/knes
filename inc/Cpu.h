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
	int8_t opCycle{ 0 };
	uint8_t op{ 0 };
	uint8_t immVal{ 0 };
	uint8_t operand{ 0 };
	int8_t branchOffset{ 0 };
	uint16_t workingAddr{ 0 };
	uint16_t properAddr{ 0 };
	bool crossedPageBoundary{ false };

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

	void ResetOpCycles();

	void Imp();					//implied
	void Imm();					//immediate
	void Abs();					//absolute
	void Abx();					//absolute, x indexed
	void Aby();					//absolute, y indexed
	void Zpg();					//zero page
	void Zpx();					//zero page, x indexed
	void Zpy();					//zero page, y indexed
	void Rel(bool condition);	//relative (branch)
	void Idx();					//indexed indirect, x
	void Idy();					//indirect indexed, y

	void NOP();
	void ORA();
	void AND();
	void EOR();
	void ADC();
	void STA();
	void LDA();
	void STX();
	void LDX();
	void STY();
	void LDY();
	void CMP();
	void CPX();
	void CPY();
	void SBC();
	void BIT();

	void ASLA();
	void ROLA();
	void LSRA();
	void RORA();

	void ASL();
	void ROL();
	void LSR();
	void ROR();
	void DEC();
	void INC();

	void CalcNZFlags(uint8_t val);

};

