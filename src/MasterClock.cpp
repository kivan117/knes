#include "MasterClock.h"

MasterClock::MasterClock(Cpu* in_cpu) : cpu(in_cpu)
{
	ScheduleCPU();
}

bool MasterClock::Tick()
{
	//currently returns if an entire frame has been run, so we can use vsync to slow execution speed

	//TODO:
	//make better logic for scheduling

	master_cycles += cycle_ratios[PPU_RATIO][current_region_mode]; //smallest step size is ppu in all regions
	if (master_cycles >= next_cpu_tick)
	{
		cpu->Tick();
		ScheduleCPU();
	}
	if (master_cycles > clock_speed[current_region_mode])
	{
		master_cycles -= clock_speed[current_region_mode];
		next_cpu_tick -= clock_speed[current_region_mode];
		return true;
	}

	return false;
}

void MasterClock::ScheduleCPU()
{
	next_cpu_tick += cycle_ratios[CPU_RATIO][current_region_mode];
}
