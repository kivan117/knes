#include "Mmu.h"
#include "Logger.h"

Mmu::Mmu()
{
}

uint8_t Mmu::CpuReadByte(uint16_t addr)
{
	if (addr < 0x2000)
		return InternalRam[addr & 0x7FF];

	if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		LOG_TRACE("Reading byte from PPU reg: {}", addr);
		return 0xFF;
	}

	if (addr >= 0x4000 && addr <= 0x401F)
	{
		LOG_TRACE("Reading byte from IO reg: {}", addr);
		return 0xFF;
	}

	if (addr > 0x401F)
		return Cart->CpuReadByte(addr);

	LOG_ERROR("Reading byte from invalid addr: {}", addr);
	return uint8_t();
}

void Mmu::CpuWriteByte(uint16_t addr, uint8_t val)
{
	if (addr < 0x2000)
	{
		InternalRam[addr & 0x7FF] = val;
		return;
	}

	if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		LOG_TRACE("Writing byte to PPU reg: {}", addr);
		return;
	}

	if (addr >= 0x4000 && addr <= 0x401F)
	{
		LOG_TRACE("Writing byte to IO reg: {}", addr);
		return;
	}

	if (addr > 0x6000)
	{
		Cart->CpuWriteByte(addr, val);
		//blargg test printing
		//if(addr == 0x6000)
		//	LOG_DEBUG("Status: {:X}", (unsigned)CpuReadByte(0x6000));
		//else
		//{
		//	static std::string output{ "" };
		//	output.clear();
		//	for (int i = 0x6004; i < 0x6100; i++)
		//	{
		//		unsigned char letter = CpuReadByte(i);
		//		if (letter == '\0')
		//			break;
		//		output += letter;
		//	}
		//	LOG_DEBUG("{}", output.c_str());
		//}
		return;
	}

	if (addr > 0x401F)
	{
		Cart->CpuWriteByte(addr, val);
		return;
	}

	LOG_ERROR("Writing byte to invalid addr: {}", addr);
	return;
}

uint16_t Mmu::CpuReadWord(uint16_t addr)
{
	return ((uint16_t)(CpuReadByte(addr + 1) << 8) | CpuReadByte(addr));
}

void Mmu::CpuWriteWord(uint16_t addr, uint16_t val)
{
	CpuWriteByte(addr, (val & 0x0F));
	CpuWriteByte(addr + 1, (val & 0xF0) >> 8);
}

uint8_t Mmu::PpuReadByte(uint16_t addr)
{
	/*
	$0000-$0FFF 	$1000 	Pattern table 0
	$1000-$1FFF 	$1000 	Pattern table 1
	$2000-$23FF 	$0400 	Nametable 0
	$2400-$27FF 	$0400 	Nametable 1
	$2800-$2BFF 	$0400 	Nametable 2
	$2C00-$2FFF 	$0400 	Nametable 3
	$3000-$3EFF 	$0F00 	Mirrors of $2000-$2EFF
	$3F00-$3F1F 	$0020 	Palette RAM indexes
	$3F20-$3FFF 	$00E0 	Mirrors of $3F00-$3F1F 

	$0000-1FFF is normally mapped by the cartridge to a CHR-ROM or CHR-RAM, often with a bank switching mechanism.
	$2000-2FFF is normally mapped to the 2kB NES internal VRAM, providing 2 nametables with a mirroring configuration controlled by the cartridge, but it can be partly or fully remapped to RAM on the cartridge, allowing up to 4 simultaneous nametables.
	$3000-3EFF is usually a mirror of the 2kB region from $2000-2EFF. The PPU does not render from this address range, so this space has negligible utility.
	$3F00-3FFF is not configurable, always mapped to the internal palette control.
	*/

	LOG_ERROR("Read from PPU bus. Unimplemented.");
	exit(-1);
	return uint8_t();
}

void Mmu::PpuWriteByte(uint16_t addr, uint8_t val)
{
	LOG_ERROR("Write to PPU bus. Unimplemented.");
	exit(-1);
}

uint16_t Mmu::PpuReadWord(uint16_t addr)
{
	LOG_ERROR("Read from PPU bus. Unimplemented.");
	exit(-1);
	return uint16_t();
}

void Mmu::PpuWriteWord(uint16_t addr, uint16_t val)
{
	LOG_ERROR("Write to PPU bus. Unimplemented.");
	exit(-1);
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
