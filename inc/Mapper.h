#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include "NESHeader.h"

class Mapper
{
public:
	virtual uint8_t CpuReadByte(uint16_t addr) const;
	virtual void CpuWriteByte(uint16_t addr, uint8_t val);
	virtual uint16_t CpuReadWord(uint16_t addr) const;
	virtual void CpuWriteWord(uint16_t addr, uint16_t val);

	virtual uint8_t PpuReadByte(uint16_t addr) const;
	virtual void PpuWriteByte(uint16_t addr, uint8_t val);
	virtual uint16_t PpuReadWord(uint16_t addr) const;
	virtual void PpuWriteWord(uint16_t addr, uint16_t val);

	void SetCartHeader(NESHeader& header);

	void LoadGame(std::istream& inFile);
	void LoadGame(std::istream& inFile, NESHeader& header);
protected:
	std::vector<std::array<uint8_t, 0x4000>> PRG_ROM;
	std::vector<uint8_t> PRG_RAM;
	std::vector<std::array<uint8_t, 0x2000>> CHR_ROM;
	std::vector<std::array<uint8_t, 0x2000>> CHR_RAM;
	
	NESHeader CartHeader;

	uint8_t prgRomBanks{ 1 };
	uint8_t prgRamBanks{ 0 };

};

