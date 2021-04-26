#include "Mapper000.h"
#include "Logger.h"

//Mapper 000, NROM

Mapper000::Mapper000()
{
	PRG_ROM.resize(1); //2 banks, 16kb each
	//PRG_RAM.resize(0x1000); //no banks, 4kb total, only needed for 'family basic' apparently
	//CHR_RAM.resize(1); //1 bank, 8kb
	
	//Just some default header data. Should be overridden on loading a game
	CartHeader.chrRamBanks = 1;
	CartHeader.prgRomBanks = 1;
	CartHeader.chrRomBanks = 0;
	CartHeader.trainerExists = false;
}

uint8_t Mapper000::CpuReadByte(uint16_t addr) const
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
		return PRG_RAM[addr & 0x0FFF];

	if (addr >= 0x8000 && addr <= 0xBFFF)
		return PRG_ROM[0][addr & 0x3FFF];

	if (addr >= 0xC000 && addr <= 0xFFFF)
	{
		if(prgRomBanks == 1)
			return PRG_ROM[0][addr & 0x3FFF];
		return PRG_ROM[1][addr & 0x3FFF];
	}

	LOG_ERROR("Cart read from invalid addr: {}", addr);
	return uint8_t();
}

void Mapper000::CpuWriteByte(uint16_t addr, uint8_t val)
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		PRG_RAM[addr & 0x0FFF] = val;
		return;
	}

	LOG_ERROR("Cart write to invalid addr: {}", addr);
}

uint16_t Mapper000::CpuReadWord(uint16_t addr) const
{
	return ((uint16_t)(ReadByte(addr + 1) << 8) | ReadByte(addr));
}

void Mapper000::CpuWriteWord(uint16_t addr, uint16_t val)
{
	WriteByte(addr, (val & 0x0F));
	WriteByte(addr + 1, (val & 0xF0) >> 8);
}


uint8_t Mapper000::PpuReadByte(uint16_t addr) const
{
	LOG_ERROR("Ppu Read not implemented: {}", addr);
	return uint8_t();
}

void Mapper000::PpuWriteByte(uint16_t addr, uint8_t val)
{
	LOG_ERROR("Ppu Write not implemented: {}", addr);
}

uint16_t Mapper000::PpuReadWord(uint16_t addr) const
{
	LOG_ERROR("Ppu Read not implemented: {}", addr);
	return uint16_t();
}

void Mapper000::PpuWriteWord(uint16_t addr, uint16_t val)
{
	LOG_ERROR("Ppu Write not implemented: {}", addr);
}
