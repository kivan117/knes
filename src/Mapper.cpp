#include "Mapper.h"
#include "Logger.h"

uint8_t Mapper::CpuReadByte(uint16_t addr) const
{
	LOG_ERROR("Cpu Read with invalid default mapper: {}", addr);
	return uint8_t();
}

void Mapper::CpuWriteByte(uint16_t addr, uint8_t val)
{
	LOG_ERROR("Cpu Write with invalid default mapper: {}", addr);
}

uint16_t Mapper::CpuReadWord(uint16_t addr) const
{
	LOG_ERROR("Cpu Read with invalid default mapper: {}", addr);
	return uint16_t();
}

void Mapper::CpuWriteWord(uint16_t addr, uint16_t val)
{
	LOG_ERROR("Cpu Write with invalid default mapper: {}", addr);
}

uint8_t Mapper::PpuReadByte(uint16_t addr) const
{
	LOG_ERROR("Ppu Read with invalid default mapper: {}", addr);
	return uint8_t();
}

void Mapper::PpuWriteByte(uint16_t addr, uint8_t val)
{
	LOG_ERROR("Ppu Write with invalid default mapper: {}", addr);
}

uint16_t Mapper::PpuReadWord(uint16_t addr) const
{
	LOG_ERROR("Ppu Read with invalid default mapper: {}", addr);
	return uint16_t();
}

void Mapper::PpuWriteWord(uint16_t addr, uint16_t val)
{
	LOG_ERROR("Ppu Write with invalid default mapper: {}", addr);
}

void Mapper::SetCartHeader(NESHeader& header)
{
	CartHeader = header;
}

void Mapper::LoadGame(std::istream& inFile)
{
	LoadGame(inFile, CartHeader);
}

void Mapper::LoadGame(std::istream& inFile, NESHeader& header)
{
	inFile.seekg(0, std::ios::end);
	uint64_t totalFileSize = inFile.tellg();
	inFile.clear();

	if(header.trainerExists)
		inFile.seekg(528, std::ios::beg);
	else
		inFile.seekg(16, std::ios::beg);

	prgRomBanks = header.prgRomBanks;
	PRG_ROM.resize(header.prgRomBanks);

	for (int n = 0; n < PRG_ROM.size(); n++)
	{
		inFile.read((char*)PRG_ROM[n].data(), 0x4000);
	}

	CHR_ROM.resize(header.chrRomBanks);

	for (int n = 0; n < CHR_ROM.size(); n++)
	{
		inFile.read((char*)CHR_ROM[n].data(), 0x2000);
	}

	prgRamBanks = header.prgRamBanks;
	if(header.prgRamBanks > 0)
		PRG_RAM.resize(0x1000);

	CHR_RAM.resize(header.chrRamBanks);

	LOG_INFO("Load game complete.");
}
