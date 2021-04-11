#include "Mapper.h"
#include "Logger.h"

uint8_t Mapper::ReadByte(uint16_t addr) const
{
	LOG_ERROR("Read with invalid default mapper: {}", addr);
	return uint8_t();
}

void Mapper::WriteByte(uint16_t addr, uint8_t val)
{
	LOG_ERROR("Write with invalid default mapper: {}", addr);
}

uint16_t Mapper::ReadWord(uint16_t addr) const
{
	LOG_ERROR("Read with invalid default mapper: {}", addr);
	return uint16_t();
}

void Mapper::WriteWord(uint16_t addr, uint16_t val)
{
	LOG_ERROR("Write with invalid default mapper: {}", addr);
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

	if(header.prgRamBanks > 0)
		PRG_RAM.resize(0x1000);

	CHR_RAM.resize(header.chrRamBanks);

	LOG_INFO("Load game complete.");
}
