#pragma once
#include "Mapper.h"

//Mapper 000, NROM

class Mapper000 : public Mapper
{
public:
	Mapper000();
	uint8_t ReadByte(uint16_t addr) const override;
	void WriteByte(uint16_t addr, uint8_t val) override;
	uint16_t ReadWord(uint16_t addr) const override;
	void WriteWord(uint16_t addr, uint16_t val) override;
private:
	uint8_t prgRomBanks{ 1 };
	uint8_t prgRamBanks{ 0 };
};

