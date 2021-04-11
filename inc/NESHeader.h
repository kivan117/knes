#pragma once
#include <cstdint>
class NESHeader
{
public:
	int mapperNumber{ -1 };
	uint8_t prgRomBanks{ 1 };
	uint8_t prgRamBanks{ 0 };
	uint8_t chrRomBanks{ 0 };
	uint8_t chrRamBanks{ 0 };
	bool trainerExists{ false };
};