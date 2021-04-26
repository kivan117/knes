#pragma once
#include <cstdint>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Mapper.h"
#include "NESHeader.h"
#include "Mapper000.h"
class Mmu
{
public:
	Mmu();

	uint8_t CpuReadByte(uint16_t addr);
	void CpuWriteByte(uint16_t addr, uint8_t val);
	uint16_t CpuReadWord(uint16_t addr);
	void CpuWriteWord(uint16_t addr, uint16_t val);

	uint8_t PpuReadByte(uint16_t addr);
	void PpuWriteByte(uint16_t addr, uint8_t val);
	uint16_t PpuReadWord(uint16_t addr);
	void PpuWriteWord(uint16_t addr, uint16_t val);

	void LoadCart(std::istream& inFile);
private:
	void ProcessHeader(std::istream& inFile, NESHeader& header);
	void CreateMapper(uint16_t ines_id);

	std::array<uint8_t, 0x800>  InternalRam{ { 0 } };

	std::unique_ptr<Mapper> Cart{ nullptr };
};

