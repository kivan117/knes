#include "Mmu.h"
#include "Logger.h"

Mmu::Mmu()
{
}

uint8_t Mmu::ReadByte(uint16_t addr)
{
	if (addr < 0x2000)
		return InternalRam[addr & 0x7FF];

	if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		LOG_INFO("Reading byte from PPU reg: {}", addr);
		return 0xFF;
	}

	if (addr >= 0x4000 && addr <= 0x401F)
	{
		LOG_INFO("Reading byte from IO reg: {}", addr);
		return 0xFF;
	}

	if (addr > 0x401F)
		return Cart->ReadByte(addr);

	LOG_ERROR("Reading byte from invalid addr: {}", addr);
	return uint8_t();
}

void Mmu::WriteByte(uint16_t addr, uint8_t val)
{
	if (addr < 0x2000)
	{
		InternalRam[addr & 0x7FF] = val;
		return;
	}

	if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		LOG_INFO("Writing byte to PPU reg: {}", addr);
		return;
	}

	if (addr >= 0x4000 && addr <= 0x401F)
	{
		LOG_INFO("Writing byte to IO reg: {}", addr);
		return;
	}

	if (addr > 0x401F)
	{
		Cart->WriteByte(addr, val);
		return;
	}

	LOG_ERROR("Writing byte to invalid addr: {}", addr);
	return;
}

uint16_t Mmu::ReadWord(uint16_t addr)
{
	return ((uint16_t)(ReadByte(addr + 1) << 8) | ReadByte(addr));
}

void Mmu::WriteWord(uint16_t addr, uint16_t val)
{
	WriteByte(addr, (val & 0x0F));
	WriteByte(addr + 1, (val & 0xF0) >> 8);
}

void Mmu::LoadCart(std::istream& inFile)
{
	NESHeader newHeader;
	ProcessHeader(inFile, newHeader);
	CreateMapper(newHeader.mapperNumber);
	Cart->SetCartHeader(newHeader);
	Cart->LoadGame(inFile);
}

void Mmu::ProcessHeader(std::istream& inFile, NESHeader& header)
{
	inFile.seekg(0, std::ios::beg);
	char headerCheck[5]{0};
	char goodHeader[5]{ 0x4E, 0x45, 0x53, 0x1A, 0x00 };
	inFile.read(headerCheck, 4);
	if (strcmp(headerCheck,goodHeader) != 0)
	{
		LOG_ERROR("File is not iNES compatible rom.");
		return;
	}
	char tempByte;

	inFile.read(&tempByte, 1);
	header.prgRomBanks = tempByte;
	inFile.read(&tempByte, 1);
	header.chrRomBanks = tempByte;
	header.chrRamBanks = (header.chrRomBanks == 0) ? 1 : 0;

	inFile.read(&tempByte, 1);
	
	header.prgRamBanks   = (tempByte & 0x02) > 0 ? 1 : 0;
	header.trainerExists = (tempByte & 0x04) > 0 ? true : false;

	uint8_t mapperByte{ 0 };
	mapperByte |= (tempByte >> 4);

	inFile.read(&tempByte, 1);

	mapperByte |= (tempByte & 0xF0);

	header.mapperNumber = mapperByte;
}

void Mmu::CreateMapper(uint16_t ines_id)
{
	switch (ines_id)
	{
	case(0):
		Cart = std::make_unique<Mapper000>();
		break;
	default:
		LOG_ERROR("Mapper number unknown: {}", ines_id);
		Cart = std::make_unique<Mapper000>();
		break;
	}	
}
