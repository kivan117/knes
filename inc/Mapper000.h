#pragma once
#include "Mapper.h"

//Mapper 000, NROM

class Mapper000 : public Mapper
{
public:
	Mapper000();
	uint8_t CpuReadByte(uint16_t addr) const override;
	void CpuWriteByte(uint16_t addr, uint8_t val) override;
	uint16_t CpuReadWord(uint16_t addr) const override;
	void CpuWriteWord(uint16_t addr, uint16_t val) override;

	uint8_t PpuReadByte(uint16_t addr) const override;
	void PpuWriteByte(uint16_t addr, uint8_t val) override;
	uint16_t PpuReadWord(uint16_t addr) const override;
	void PpuWriteWord(uint16_t addr, uint16_t val) override;
private:
	uint8_t prgRomBanks{ 1 };
	uint8_t prgRamBanks{ 0 };
};

