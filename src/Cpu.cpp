#include "Cpu.h"

Cpu::Cpu(Mmu* _mmu) : mmu(_mmu)
{
    Regs.A  = 0x00;
    Regs.X  = 0x00;
    Regs.Y  = 0x00;
    Regs.P  = 0x34;
    Regs.S  = 0xFD;
    Regs.PC = 0xC000; //for auto nestest. normal reset vector is 0xFFFC
}

void Cpu::Tick()
{
    //fetch
    uint8_t op = mmu->ReadByte(Regs.PC++);
    //decode-execute
    opCycles = Execute(op);
    totalCycles += opCycles;
}

uint8_t Cpu::Execute(uint8_t op)
{
    uint8_t tempCycles = BaseCyclesLUT[op];
    switch (op)
    {
    case(0xEA):
        //NOP
        break;
    default:
        LOG_ERROR("Invalid opcode: {}", op);
        exit(-1);
    }

    return tempCycles;
}

void Cpu::SetFlag(FLAG_CODE flagCode, uint8_t val)
{
    switch (flagcode)
    {
    case(FLAG_C):
        Flags.carry = (val != 0) ? true : false;
        break;
    case(FLAG_Z):
        Flags.zero = (val != 0) ? true : false;
        break;
    case(FLAG_I):
        Flags.irq_disable = (val != 0) ? true : false;
        break;
    case(FLAG_D):
        Flags.decimal = (val != 0) ? true : false;
        break;
    case(FLAG_B):
        Flags.brk_interrupt = (val != 0) ? true : false;
        break;
    case(FLAG_V):
        Flags.overflow = (val != 0) ? true : false;
        break;
    case(FLAG_N):
        Flags.negative = (val != 0) ? true : false;
        break;
    default:
        LOG_ERROR("Attempt to set improper flag: {}", flagCode);
        exit(-1);
    }

    uint8_t mask = 1 << flagCode;
    Regs.P &= ~(mask);
    Regs.P |= (val != 0) ? mask : 0;
}
