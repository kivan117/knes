#pragma once
#include "Cpu.h"
class MasterClock
{
public:
	MasterClock(Cpu* in_cpu);
	bool Tick();
private:
	void ScheduleCPU();
private:
	Cpu* cpu;
	enum RegionMode	{NTSC, PAL, CLONE, NUM_MODES};
	RegionMode current_region_mode{NTSC};
	uint64_t master_cycles{ 0 };
	//uint64_t clock_speed[3]{ 21477276, 26601680, 26601680 }; //master clock ticks per second. NTSC, PAL, Dendy Clone
	uint64_t clock_speed[3]{ 357954, 532033, 532033 }; //master clock ticks per frame, roughly.
	uint64_t next_cpu_tick{ 12 };
	uint64_t next_ppu_tick{ 0 };
	uint64_t next_apu_tick{ 0 };
	//const uint8_t  cpu_cycle_ratio[4]{ 12, 16, 15, 12 };
	//const uint8_t  ppu_cycle_ratio[4]{ 4,  5,  5,  4 };
	//const uint8_t  apu_cycle_ratio[4]{ 12, 16, 15, 12 };
	enum CycleRatios {CPU_RATIO, PPU_RATIO, APU_RATIO, NUM_RATIOS};
	const std::array< std::array< uint8_t, 3>, 3> cycle_ratios{ {
		{ 12, 16, 15 }, //cpu ratios
		{  4,  5,  5 }, //ppu ratios
		{ 12, 16, 15 }  //apu ratios, should always match cpu
	} };
};

