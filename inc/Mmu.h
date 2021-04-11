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

	uint8_t ReadByte(uint16_t addr);
	void WriteByte(uint16_t addr, uint8_t val);
	uint16_t ReadWord(uint16_t addr);
	void WriteWord(uint16_t addr, uint16_t val);

	void LoadCart(std::istream& inFile);
private:
	void ProcessHeader(std::istream& inFile, NESHeader& header);
	void CreateMapper(uint16_t ines_id);

	std::array<uint8_t, 0x800>  InternalRam{ { 0 } };
	std::unique_ptr<Mapper> Cart{ nullptr };
};

