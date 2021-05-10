#include "Cpu.h"

Cpu::Cpu(Mmu* _mmu) : mmu(_mmu)
{
    Regs.A  = 0x00;
    Regs.X  = 0x00;
    Regs.Y  = 0x00;
    Regs.P  = 0x24;
    Regs.S  = 0xFD;
    Regs.PC = 0xC000; //for auto nestest. normal reset vector is 0xFFFC
}

void Cpu::Tick()
{
    
    if (opCycle < 0)
    {
        //fetch
        op = mmu->CpuReadByte(Regs.PC++); //op cycle 1
        //PrintDebugString();
        CheckDebugStatus(debugLogLine++);
        opCycle = 2;
    }
    else
    {
        //decode-execute
        Execute(op);
    }
    totalCycles++;
}

void Cpu::PrintDebugString()
{
    std::stringstream stream;
    stream  << std::setfill('0') << std::setw(4)
            << std::hex << (Regs.PC - 1) << "\t";
    for (int i = 0; i < OPCODE_INFO_TABLE[op].length; i++)
    {
        stream  << std::setfill('0') << std::setw(2)
                << std::hex << mmu->CpuReadByte(Regs.PC - 1 + i) << " ";
    }
    stream << "\t";
    stream << "A:" << std::setfill('0') << std::setw(2)
        << std::hex << (Regs.A) << " ";
    stream << "X:" << std::setfill('0') << std::setw(2)
        << std::hex << (Regs.X) << " ";
    stream << "Y:" << std::setfill('0') << std::setw(2)
        << std::hex << (Regs.Y) << " ";
    stream << "P:" << std::setfill('0') << std::setw(2)
        << std::hex << (Regs.P) << " ";
    stream << "SP:" << std::setfill('0') << std::setw(2)
        << std::hex << (Regs.S);
    LOG_TRACE(stream.str().c_str());
}

void Cpu::CheckDebugStatus(unsigned int which)
{
    if (Regs.PC - 1 != ARR_PC[which])
    {
        LOG_ERROR("nestest failure on line {}: PC", which);
        LOG_ERROR("Expected: {}", ARR_PC[which]);
        LOG_ERROR("Actual:   {}", Regs.PC - 1);
        exit(-2);
    }
    if (op != ARR_OPCODE[which])
    {
        LOG_ERROR("nestest failure on line {}: OP", which);
        LOG_ERROR("Expected: {}", ARR_OPCODE[which]);
        LOG_ERROR("Actual:   {}", op);
        exit(-2);
    }
    if (Regs.A != ARR_A[which])
    {
        LOG_ERROR("nestest failure on line {}: A", which);
        LOG_ERROR("Expected: {}", ARR_A[which]);
        LOG_ERROR("Actual:   {}", Regs.A);
        exit(-2);
    }
    if (Regs.X != ARR_X[which])
    {
        LOG_ERROR("nestest failure on line {}: X", which);
        LOG_ERROR("Expected: {}", ARR_X[which]);
        LOG_ERROR("Actual:   {}", Regs.X);
        exit(-2);
    }
    if (Regs.Y != ARR_Y[which])
    {
        LOG_ERROR("nestest failure on line {}: Y", which);
        LOG_ERROR("Expected: {}", ARR_Y[which]);
        LOG_ERROR("Actual:   {}", Regs.Y);
        exit(-2);
    }
    if (Regs.P != ARR_P[which])
    {
        LOG_ERROR("nestest failure on line {}: P", which);
        LOG_ERROR("Expected: {}", ARR_P[which]);
        LOG_ERROR("Actual:   {}", Regs.P);
        exit(-2);
    }
    if (Regs.S != ARR_SP[which])
    {
        LOG_ERROR("nestest failure on line {}: SP", which);
        LOG_ERROR("Expected: {}", ARR_SP[which]);
        LOG_ERROR("Actual:   {}", Regs.S);
        exit(-2);
    }
}

void Cpu::SetFlag(FLAG_CODE flagCode, uint8_t val)
{
    switch (flagCode)
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
    case(FLAG_HIGH):
        break;
    default:
        LOG_ERROR("Attempt to set improper flag: {}", flagCode);
        exit(-1);
    }

    uint8_t mask = 1 << flagCode;
    Regs.P &= ~(mask);
    Regs.P |= (val != 0) ? mask : 0;

    return;
}

void Cpu::SyncFlagsFromReg()
{
    uint8_t val = (Regs.P & (1 << FLAG_N)) >> FLAG_N;
    SetFlag(FLAG_N, val);
    val = (Regs.P & (1 << FLAG_V)) >> FLAG_V;
    SetFlag(FLAG_V, val);
    SetFlag(FLAG_HIGH, 1);
    val = (Regs.P & (1 << FLAG_B)) >> FLAG_B;
    SetFlag(FLAG_B, val);
    val = (Regs.P & (1 << FLAG_D)) >> FLAG_D;
    SetFlag(FLAG_D, val);
    val = (Regs.P & (1 << FLAG_I)) >> FLAG_I;
    SetFlag(FLAG_I, val);
    val = (Regs.P & (1 << FLAG_Z)) >> FLAG_Z;
    SetFlag(FLAG_Z, val);
    val = (Regs.P & (1 << FLAG_C)) >> FLAG_C;
    SetFlag(FLAG_C, val);
}

void Cpu::ResetOpCycles()
{
    opCycle = -2;
}

void Cpu::Imp()
{
    if (opCycle == 2)
        uint8_t dummyRead = mmu->CpuReadByte(Regs.PC);
}

void Cpu::Imm()
{
    if (opCycle == 2)
        operand = mmu->CpuReadByte(Regs.PC++);
}

void Cpu::Abs()
{
    addrModeCycleOffset = 4;
    switch (opCycle)
    {
    case(2):
        workingAddr = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        workingAddr |= (mmu->CpuReadByte(Regs.PC++) << 8);
        break;
    default: //all cycles after 3
        break;
    }
}

void Cpu::Abx()
{
    addrModeCycleOffset = 5;
    switch (opCycle)
    {
    case(2):
        crossedPageBoundary = false;
        workingAddr = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        workingAddr |= (mmu->CpuReadByte(Regs.PC++) << 8);
        properAddr = workingAddr + Regs.X;
        workingAddr = (workingAddr & 0xFF00) | (properAddr & 0x00FF);
        crossedPageBoundary = (workingAddr != properAddr) ? true : false;
        break;
    case(4):
        operand = mmu->CpuReadByte(workingAddr);
        workingAddr = properAddr;
        break;
    default: //all cycles after 4
        break;
    }
}

void Cpu::Aby()
{
    switch (opCycle)
    {
    case(2):
        crossedPageBoundary = false;
        workingAddr = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        workingAddr |= (mmu->CpuReadByte(Regs.PC++) << 8);
        properAddr = workingAddr + Regs.Y;
        workingAddr = (workingAddr & 0xFF00) | (properAddr & 0x00FF);
        crossedPageBoundary = (workingAddr != properAddr) ? true : false;
        break;
    case(4):
        operand = mmu->CpuReadByte(workingAddr);
        workingAddr = properAddr;
        break;
    default: //all cycles after 3
        break;
    }
}

void Cpu::Zpg()
{
    addrModeCycleOffset = 3;
    if (opCycle == 2)
        workingAddr = mmu->CpuReadByte(Regs.PC++);
}

void Cpu::Zpx()
{
    addrModeCycleOffset = 4;
    switch (opCycle)
    {
    case(2):
        workingAddr = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3): {
        uint8_t dummyRead = mmu->CpuReadByte(workingAddr);
        workingAddr = (workingAddr + Regs.X) & 0xFF;
        break; 
    }
    default:
        break;
    }
}

void Cpu::Zpy()
{
    addrModeCycleOffset = 4;
    switch (opCycle)
    {
    case(2):
        workingAddr = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3): {
        uint8_t dummyRead = mmu->CpuReadByte(workingAddr);
        workingAddr = (workingAddr + Regs.Y) & 0xFF;
        break;
    }
    default:
        break;
    }
}

void Cpu::Rel(bool condition)
{
    switch (opCycle)
    {
    case(2):
        crossedPageBoundary = false;
        branchOffset = (int8_t)mmu->CpuReadByte(Regs.PC++);
        if (!condition)
            ResetOpCycles();
        break;
    case(3): {
        uint8_t dummyRead = mmu->CpuReadByte(Regs.PC);
        workingAddr = Regs.PC;
        properAddr = workingAddr + branchOffset;
        workingAddr = (workingAddr & 0xFF00) | (((workingAddr & 0x00FF) + branchOffset) & 0x00FF);
        Regs.PC = workingAddr;
        if (workingAddr == properAddr)
            ResetOpCycles();
        else
            crossedPageBoundary = true;
        break;
    }
    case(4): {
        uint8_t dummyRead = mmu->CpuReadByte(Regs.PC);
        Regs.PC = (workingAddr > properAddr) ? Regs.PC - 0x100 : Regs.PC + 0x100;
        ResetOpCycles();
        break;
    }
    default:
        break;
    }
}

void Cpu::Idx()
{
    static uint8_t pointer; //uint8_T so it's confined to the zero page automatically
    switch (opCycle)
    {
    case(2):
        pointer = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3): {
           uint8_t dummyRead = mmu->CpuReadByte(pointer);
           pointer += Regs.X;
           break;
    }
    case(4):
        workingAddr = mmu->CpuReadByte(pointer);
        break;
    case(5):
        workingAddr |= (mmu->CpuReadByte(pointer+1) << 8);
        break;
    default:
        break;
    }
}

void Cpu::Idy()
{
    static uint8_t pointer;
    switch (opCycle)
    {
    case(2):
        crossedPageBoundary = false;
        immVal = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        pointer = mmu->CpuReadByte(immVal++);
        break;
    case(4):
        pointer |= (mmu->CpuReadByte(immVal) << 8);

        workingAddr = pointer;
        properAddr = workingAddr + Regs.Y;
        workingAddr = (workingAddr & 0xFF00) | (((workingAddr & 0x00FF) + Regs.Y) & 0x00FF);
        if (workingAddr != properAddr)
            crossedPageBoundary = true;
        break;
    case(5):
        operand = mmu->CpuReadByte(workingAddr);
        workingAddr = properAddr;
        break;
    default:
        break;
    }
}

void Cpu::NOP()
{
    Imp();
    ResetOpCycles();
}

void Cpu::ORA()
{
    Regs.A |= operand;
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::AND()
{
    Regs.A &= operand;
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::EOR()
{
    Regs.A ^= operand;
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::ADC()
{
    uint8_t c = Flags.carry ? 1 : 0;
    uint16_t sum = Regs.A + operand + c;
    SetFlag(FLAG_C, (sum > 255) ? 1 : 0);
    CalcNZFlags(sum & 0xFF);
    if(((Regs.A >> 7) == (operand >> 7)) && ((Regs.A >> 7) ^ ((sum >> 7) & 1)))
        SetFlag(FLAG_V, 1);
    else
        SetFlag(FLAG_V, 0);
    Regs.A = sum & 0xFF;

    ResetOpCycles();
}

void Cpu::STA()
{
    mmu->CpuWriteByte(workingAddr, Regs.A);
    ResetOpCycles();
}

void Cpu::LDA()
{
    Regs.A = operand;
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::STX()
{
    mmu->CpuWriteByte(workingAddr, Regs.X);
    ResetOpCycles();
}

void Cpu::LDX()
{
    Regs.X = operand;
    CalcNZFlags(Regs.X);
    ResetOpCycles();
}

void Cpu::STY()
{
    mmu->CpuWriteByte(workingAddr, Regs.Y);
    ResetOpCycles();
}

void Cpu::LDY()
{
    Regs.Y = operand;
    CalcNZFlags(Regs.Y);
    ResetOpCycles();
}

void Cpu::CMP()
{
    uint8_t temp = Regs.A - operand;
    SetFlag(FLAG_Z, (temp == 0) ? 1 : 0);
    SetFlag(FLAG_N, ((temp >> 7) & 1));
    SetFlag(FLAG_C, (operand > Regs.A) ? 0 : 1);
    ResetOpCycles();
}

void Cpu::CPX()
{
    uint8_t temp = Regs.X - operand;
    SetFlag(FLAG_Z, (temp == 0) ? 1 : 0);
    SetFlag(FLAG_N, ((temp >> 7) & 1));
    SetFlag(FLAG_C, (operand > Regs.X) ? 0 : 1);
    ResetOpCycles();
}

void Cpu::CPY()
{
    uint8_t temp = Regs.Y - operand;
    SetFlag(FLAG_Z, (temp == 0) ? 1 : 0);
    SetFlag(FLAG_N, ((temp >> 7) & 1));
    SetFlag(FLAG_C, (operand > Regs.Y) ? 0 : 1);
    ResetOpCycles();
}

void Cpu::SBC()
{
    operand = ~operand;
    ADC();
}

void Cpu::BIT()
{
    uint8_t temp = Regs.A & operand;
    (temp == 0) ? SetFlag(FLAG_Z, 1) : SetFlag(FLAG_Z, 0);
    temp = ((operand & 0x40) >> 6) & 1;
    SetFlag(FLAG_V, temp);
    temp = (operand & 0x80) >> 7;
    SetFlag(FLAG_N, temp);

    ResetOpCycles();
}

void Cpu::ASLA()
{
    SetFlag(FLAG_C, (Regs.A & 0x80) >> 7);
    Regs.A <<= 1;
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::ROLA()
{
    uint8_t tempCarry = (Regs.A & 0x80) >> 7;
    Regs.A <<= 1;
    Regs.A |= (Flags.carry ? 1 : 0);
    SetFlag(FLAG_C, tempCarry);
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::LSRA()
{
    SetFlag(FLAG_C, Regs.A & 1);
    Regs.A >>= 1;
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::RORA()
{
    uint8_t tempCarry = Regs.A & 1;
    Regs.A >>= 1;
    Regs.A |= (Flags.carry ? 1 : 0) << 7;
    SetFlag(FLAG_C, tempCarry);
    CalcNZFlags(Regs.A);
    ResetOpCycles();
}

void Cpu::ASL()
{
    if (opCycle == addrModeCycleOffset)
    {
        operand = mmu->CpuReadByte(workingAddr);
    }
    else if (opCycle == addrModeCycleOffset + 1)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        SetFlag(FLAG_C, (operand & 0x80) >> 7);
        operand <<= 1;
        CalcNZFlags(operand);
    }
    else if (opCycle == addrModeCycleOffset + 2)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        ResetOpCycles();
    }    
}

void Cpu::ROL()
{
    if (opCycle == addrModeCycleOffset)
    {
        operand = mmu->CpuReadByte(workingAddr);
    }
    else if (opCycle == addrModeCycleOffset + 1)
    {
        mmu->CpuWriteByte(workingAddr, operand);

        uint8_t tempCarry = (operand & 0x80) >> 7;
        operand <<= 1;
        operand |= (Flags.carry ? 1 : 0);
        SetFlag(FLAG_C, tempCarry);
        CalcNZFlags(operand);

    }
    else if (opCycle == addrModeCycleOffset + 2)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        ResetOpCycles();
    }
}

void Cpu::LSR()
{
    if (opCycle == addrModeCycleOffset)
    {
        operand = mmu->CpuReadByte(workingAddr);
    }
    else if (opCycle == addrModeCycleOffset + 1)
    {
        mmu->CpuWriteByte(workingAddr, operand);

        SetFlag(FLAG_C, operand & 1);
        operand >>= 1;
        CalcNZFlags(operand);
    }
    else if (opCycle == addrModeCycleOffset + 2)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        ResetOpCycles();
    }
}

void Cpu::ROR()
{
    if (opCycle == addrModeCycleOffset)
    {
        operand = mmu->CpuReadByte(workingAddr);
    }
    else if (opCycle == addrModeCycleOffset + 1)
    {
        mmu->CpuWriteByte(workingAddr, operand);

        uint8_t tempCarry = operand & 1;
        operand >>= 1;
        operand |= (Flags.carry ? 1 : 0) << 7;
        SetFlag(FLAG_C, tempCarry);
        CalcNZFlags(operand);

    }
    else if (opCycle == addrModeCycleOffset + 2)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        ResetOpCycles();
    }
}

void Cpu::DEC()
{
    if (opCycle == addrModeCycleOffset)
    {
        operand = mmu->CpuReadByte(workingAddr);
    }
    else if (opCycle == addrModeCycleOffset + 1)
    {
        mmu->CpuWriteByte(workingAddr, operand);

        operand -= 1;
        CalcNZFlags(operand);

    }
    else if (opCycle == addrModeCycleOffset + 2)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        ResetOpCycles();
    }
}

void Cpu::INC()
{
    if (opCycle == addrModeCycleOffset)
    {
        operand = mmu->CpuReadByte(workingAddr);
    }
    else if (opCycle == addrModeCycleOffset + 1)
    {
        mmu->CpuWriteByte(workingAddr, operand);

        operand += 1;
        CalcNZFlags(operand);

    }
    else if (opCycle == addrModeCycleOffset + 2)
    {
        mmu->CpuWriteByte(workingAddr, operand);
        ResetOpCycles();
    }
}

void Cpu::JMPABS()
{
    switch (opCycle)
    {
    case(2):
        workingAddr = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        workingAddr |= (mmu->CpuReadByte(Regs.PC) << 8);
        Regs.PC = workingAddr;
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::JMPIND()
{
    static uint16_t pointer{ 0 };
    switch (opCycle)
    {
    case(2):
        pointer = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        pointer |= mmu->CpuReadByte(Regs.PC++) << 8;
        break;
    case(4):
        workingAddr = mmu->CpuReadByte(pointer);
        break;
    case(5):
        workingAddr |= mmu->CpuReadByte((pointer & 0xFF00) | ((pointer + 1) & 0xFF)) << 8;
        Regs.PC = workingAddr;
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::PHA()
{
    switch (opCycle)
    {
    case(2): {
        uint8_t dummyRead = mmu->CpuReadByte(Regs.PC);
        break;
    }
    case(3):
        mmu->CpuWriteByte(Regs.S + 0x0100, Regs.A);
        Regs.S--;
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::PLA()
{
    static uint8_t dummyRead{ 0 };
    switch (opCycle)
    {
    case(2):
        dummyRead = mmu->CpuReadByte(Regs.PC);
        break;
    case(3):
        dummyRead = mmu->CpuReadByte(Regs.S + 0x0100);
        Regs.S++;
        break;
    case(4):
        Regs.A = mmu->CpuReadByte(Regs.S + 0x0100);
        CalcNZFlags(Regs.A);
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::PHP()
{
    switch (opCycle)
    {
    case(2): {
        uint8_t dummyRead = mmu->CpuReadByte(Regs.PC);
        break;
    }
    case(3):
        mmu->CpuWriteByte(Regs.S + 0x0100, Regs.P | 0x30);
        Regs.S--;
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::PLP()
{
    static uint8_t dummyRead{ 0 };
    switch (opCycle)
    {
    case(2):
        dummyRead = mmu->CpuReadByte(Regs.PC);
        break;
    case(3):
        dummyRead = mmu->CpuReadByte(Regs.S + 0x0100);
        Regs.S++;
        break;
    case(4):
        Regs.P &= 0x30;
        Regs.P |= (mmu->CpuReadByte(Regs.S + 0x0100) & (~0x30));
        SyncFlagsFromReg();
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::BRK()
{
    static uint8_t dummyRead{ 0 };
    static uint8_t tempVal{ 0 };
    switch (opCycle)
    {
    case(2):
        dummyRead = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3): //push pch
        tempVal = (Regs.PC & 0xFF00) >> 8;
        mmu->CpuWriteByte(Regs.S + 0x0100, tempVal);
        Regs.S--;
        break;
    case(4): //push pcl
        tempVal = (Regs.PC & 0xFF);
        mmu->CpuWriteByte(Regs.S + 0x0100, tempVal);
        Regs.S--;
        break;
    case(5): //push p (with B set)
        tempVal = Regs.P | (1 << FLAG_B);
        mmu->CpuWriteByte(Regs.S + 0x0100, tempVal);
        Regs.S--;
        break;
    case(6):
        Regs.PC = (Regs.PC & 0xFF00) | mmu->CpuReadByte(0xFFFE);
        break;
    case(7):
        Regs.PC = (Regs.PC & 0xFF) | (mmu->CpuReadByte(0xFFFF) << 8);
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::RTI()
{
    static uint8_t dummyRead{ 0 };
    switch (opCycle)
    {
    case(2):
        dummyRead = mmu->CpuReadByte(Regs.PC);
        break;
    case(3):
        dummyRead = mmu->CpuReadByte(Regs.S + 0x0100);
        Regs.S++;
        break;
    case(4):
        Regs.P &= 0x30;
        Regs.P |= (mmu->CpuReadByte(Regs.S + 0x0100)&(~0x30));
        SyncFlagsFromReg();
        Regs.S++;
        break;
    case(5):
        workingAddr = mmu->CpuReadByte(Regs.S + 0x0100);
        Regs.PC = (Regs.PC & 0xFF00) | (workingAddr);
        Regs.S++;
        break;
    case(6):
        workingAddr |= mmu->CpuReadByte(Regs.S + 0x0100) << 8;
        Regs.PC = workingAddr;
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::JSR()
{
    static uint8_t pclow{ 0 };
    static uint8_t tempVal{ 0 };
    switch (opCycle)
    {
    case(2):
        pclow = mmu->CpuReadByte(Regs.PC++);
        break;
    case(3):
        mmu->CpuReadByte(Regs.S + 0x0100);
        break;
    case(4): //push pch
        tempVal = (Regs.PC & 0xFF00) >> 8;
        mmu->CpuWriteByte(Regs.S + 0x0100, tempVal);
        Regs.S--;
        break;
    case(5): //push pcl
        tempVal = (Regs.PC & 0xFF);
        mmu->CpuWriteByte(Regs.S + 0x0100, tempVal);
        Regs.S--;
        break;
    case(6):
        Regs.PC = pclow | (mmu->CpuReadByte(Regs.PC) << 8);
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::RTS()
{
    static uint8_t dummyRead{ 0 };
    switch (opCycle)
    {
    case(2):
        dummyRead = mmu->CpuReadByte(Regs.PC);
        break;
    case(3):
        dummyRead = mmu->CpuReadByte(Regs.S + 0x0100);
        Regs.S++;
        break;
    case(4):
        workingAddr = mmu->CpuReadByte(Regs.S + 0x0100);
        Regs.PC = (Regs.PC & 0xFF00) | (workingAddr);
        Regs.S++;
        break;
    case(5):
        workingAddr |= mmu->CpuReadByte(Regs.S + 0x0100) << 8;
        Regs.PC = workingAddr;
        break;
    case(6):
        dummyRead = mmu->CpuReadByte(Regs.PC++);
        ResetOpCycles();
        break;
    default:
        break;
    }
}

void Cpu::CalcNZFlags(uint8_t val)
{
    SetFlag(FLAG_N, (val > 127) ? 1 : 0);
    SetFlag(FLAG_Z, (val == 0)  ? 1 : 0);
}

uint8_t Cpu::Execute(uint8_t op)
{
    //log pre-op

    switch (op)
    {
    case(0x00): BRK(); break; //BRK, imp        
    case(0x10): Rel(Flags.negative == false); break; //BPL, rel
    case(0x20): JSR(); break; //JSR, abs
    case(0x30): Rel(Flags.negative == true); break; //BMI, rel
    case(0x40): RTI(); break; //RTI, imp
    case(0x50): Rel(Flags.overflow == false); break; //BVC, rel
    case(0x60): RTS(); break; //RTS, imp
    case(0x70): Rel(Flags.overflow == true); break; //BVS, rel
    case(0x90): Rel(Flags.carry == false); break; //BCC, rel
    case(0xA0): Imm(); LDY(); break; //LDY, imm
    case(0xB0): Rel(Flags.carry == true); break; //BCS, rel
    case(0xC0): Imm(); CPY(); break; //CPY, imm
    case(0xD0): Rel(Flags.zero == false); break; //BNE, rel
    case(0xE0): Imm(); CPX(); break; //CPX, imm
    case(0xF0): Rel(Flags.zero == true); break; //BEQ, rel
    case(0xA2): Imm(); LDX(); break; //LDX, imm


    case(0x81): Idx(); if (opCycle == 6) { STA(); } break;                                          //STA, idx 
    case(0x01): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); ORA(); } break; //ORA, idx    
    case(0x21): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); AND(); } break; //AND, idx    
    case(0x41): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); EOR(); } break; //EOR, idx    
    case(0x61): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); ADC(); } break; //ADC, idx 
    case(0xA1): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); LDA(); } break; //LDA, idx    
    case(0xC1): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); CMP(); } break; //CMP, idx    
    case(0xE1): Idx(); if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); SBC(); } break; //SBC, idx


    case(0x91): Idy(); if (opCycle == 6) { STA(); } break;                                                                                                    //STA, idy
    case(0x11): Idy(); if (opCycle == 5 && !crossedPageBoundary) { ORA(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); ORA(); } break; //ORA, idy
    case(0x31): Idy(); if (opCycle == 5 && !crossedPageBoundary) { AND(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); AND(); } break; //AND, idy
    case(0x51): Idy(); if (opCycle == 5 && !crossedPageBoundary) { EOR(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); EOR(); } break; //EOR, idy
    case(0x71): Idy(); if (opCycle == 5 && !crossedPageBoundary) { ADC(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); ADC(); } break; //ADC, idy
    case(0xB1): Idy(); if (opCycle == 5 && !crossedPageBoundary) { LDA(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); LDA(); } break; //LDA, idy
    case(0xD1): Idy(); if (opCycle == 5 && !crossedPageBoundary) { CMP(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); CMP(); } break; //CMP, idy
    case(0xF1): Idy(); if (opCycle == 5 && !crossedPageBoundary) { SBC(); } else if (opCycle == 6) { operand = mmu->CpuReadByte(workingAddr); SBC(); } break; //SBC, idy


    case(0x85): Zpg(); if (opCycle == 3) { STA(); } break;                                          //STA, zpg
    case(0x86): Zpg(); if (opCycle == 3) { STX(); } break;                                          //STX, zpg
    case(0x84): Zpg(); if (opCycle == 3) { STY(); } break;                                          //STY, zpg
    case(0x05): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); ORA(); } break; //ORA, zpg
    case(0x25): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); AND(); } break; //AND, zpg
    case(0x45): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); EOR(); } break; //EOR, zpg
    case(0x65): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); ADC(); } break; //ADC, zpg
    case(0xA5): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); LDA(); } break; //LDA, zpg
    case(0xC5): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); CMP(); } break; //CMP, zpg
    case(0xE5): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); SBC(); } break; //SBC, zpg
    case(0xA6): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); LDX(); } break; //LDX, zpg
    case(0xA4): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); LDY(); } break; //LDY, zpg
    case(0x24): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); BIT(); } break; //BIT, zpg
    case(0xC4): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); CPY(); } break; //CPY, zpg
    case(0xE4): Zpg(); if (opCycle == 3) { operand = mmu->CpuReadByte(workingAddr); CPX(); } break; //CPX, zpg


    case(0x95): Zpx(); if (opCycle == 4) { STA(); } break;                                          //STA, zpx
    case(0x96): Zpy(); if (opCycle == 4) { STX(); } break;                                          //STX, zpy
    case(0x94): Zpx(); if (opCycle == 4) { STY(); } break;                                          //STY, zpx
    case(0x15): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); ORA(); } break; //ORA, zpx    
    case(0x35): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); AND(); } break; //AND, zpx    
    case(0x55): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); EOR(); } break; //EOR, zpx    
    case(0x75): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); ADC(); } break; //ADC, zpx    
    case(0xB5): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); LDA(); } break; //LDA, zpx    
    case(0xD5): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); CMP(); } break; //CMP, zpx    
    case(0xF5): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); SBC(); } break; //SBC, zpx
    case(0xB6): Zpy(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); LDX(); } break; //LDX, zpy
    case(0xB4): Zpx(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr); LDY(); } break; //LDY, zpx



    case(0x06): Zpg(); ASL(); break; //ASL, zpg
    case(0x26): Zpg(); ROL(); break; //ROL, zpg
    case(0x46): Zpg(); LSR(); break; //LSR, zpg
    case(0x66): Zpg(); ROR(); break; //ROR, zpg    
    case(0xC6): Zpg(); DEC(); break; //DEC, zpg
    case(0xE6): Zpg(); INC(); break; //INC, zpg


    case(0x16): Zpx(); ASL(); break; //ASL, zpx
    case(0x36): Zpx(); ROL(); break; //ROL, zpx
    case(0x56): Zpx(); LSR(); break; //LSR, zpx
    case(0x76): Zpx(); ROR(); break; //ROR, zpx    
    case(0xD6): Zpx(); DEC(); break; //DEC, zpx
    case(0xF6): Zpx(); INC(); break; //INC, zpx

    case(0x09): Imm(); ORA(); break; //ORA, imm
    case(0x29): Imm(); AND(); break; //AND, imm
    case(0x49): Imm(); EOR(); break; //EOR, imm
    case(0x69): Imm(); ADC(); break; //ADC, imm
    case(0xA9): Imm(); LDA(); break; //LDA, imm
    case(0xC9): Imm(); CMP(); break; //CMP, imm
    case(0xE9): Imm(); SBC(); break; //SBC, imm

    case(0x08): PHP(); break; //PHP, imp
    case(0x18): Imp(); SetFlag(FLAG_C, 0); ResetOpCycles(); break; //CLC, imp
    case(0x28): PLP(); break; //PLP, imp
    case(0x38): Imp(); SetFlag(FLAG_C, 1); ResetOpCycles(); break; //SEC, imp
    case(0x48): PHA(); break; //PHA, imp
    case(0x58): Imp(); SetFlag(FLAG_I, 0); ResetOpCycles(); break; //CLI, imp
    case(0x68): PLA(); break; //PLA, imp
    case(0x78): Imp(); SetFlag(FLAG_I, 1); ResetOpCycles(); break; //SEI, imp
    case(0x88): Imp(); Regs.Y--; CalcNZFlags(Regs.Y); ResetOpCycles(); break; //DEY, imp
    case(0x98): Imp(); Regs.A = Regs.Y; CalcNZFlags(Regs.A); ResetOpCycles(); break; //TYA, imp
    case(0xA8): Imp(); Regs.Y = Regs.A; CalcNZFlags(Regs.Y); ResetOpCycles(); break; //TAY, imp
    case(0xB8): Imp(); SetFlag(FLAG_V, 0); ResetOpCycles(); break; //CLV, imp
    case(0xC8): Imp(); Regs.Y++; CalcNZFlags(Regs.Y); ResetOpCycles(); break; //INY, imp
    case(0xD8): Imp(); SetFlag(FLAG_D, 0); ResetOpCycles(); break; //CLD, imp
    case(0xE8): Imp(); Regs.X++; CalcNZFlags(Regs.X); ResetOpCycles(); break; //INX, imp
    case(0xF8): Imp(); SetFlag(FLAG_D, 1); ResetOpCycles(); break; //SED, imp

    case(0x0A): Imp(); ASLA(); break; //ASL, imp(A)
    case(0x2A): Imp(); ROLA(); break; //ROL, imp(A)    
    case(0x4A): Imp(); LSRA(); break; //LSR, imp(A)    
    case(0x6A): Imp(); RORA(); break; //ROR, imp(A)    
    case(0x8A): Imp(); Regs.A = Regs.X; CalcNZFlags(Regs.A); ResetOpCycles(); break; //TXA, imp
    case(0x9A): Imp(); Regs.S = Regs.X; ResetOpCycles(); break; //TXS, imp
    case(0xAA): Imp(); Regs.X = Regs.A; CalcNZFlags(Regs.X); ResetOpCycles();  break; //TAX, imp
    case(0xBA): Imp(); Regs.X = Regs.S; CalcNZFlags(Regs.X); ResetOpCycles();  break; //TSX, imp
    case(0xCA): Imp(); Regs.X--; CalcNZFlags(Regs.X); ResetOpCycles(); break; //DEX, imp    
    case(0xEA): NOP(); break; //NOP, imp
    
    case(0x4C): JMPABS(); break; //JMP, abs
    case(0x6C): JMPIND(); break; //JMP, ind    

    case(0x8D): Abs(); if (opCycle == 4) { STA(); } break;                                           //STA, abs
    case(0x8E): Abs(); if (opCycle == 4) { STX(); } break;                                           //STX, abs
    case(0x8C): Abs(); if (opCycle == 4) { STY(); } break;                                           //STY, abs
    case(0x0D): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  ORA(); } break; //ORA, abs
    case(0x2D): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  AND(); } break; //AND, abs
    case(0x4D): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  EOR(); } break; //EOR, abs
    case(0x6D): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  ADC(); } break; //ADC, abs    
    case(0xAD): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  LDA(); } break; //LDA, abs
    case(0xCD): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  CMP(); } break; //CMP, abs
    case(0xED): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  SBC(); } break; //SBC, abs    
    case(0xAE): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  LDX(); } break; //LDX, abs
    case(0xAC): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  LDY(); } break; //LDY, abs
    case(0xCC): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  CPY(); } break; //CPY, abs
    case(0xEC): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  CPX(); } break; //CPX, abs
    case(0x2C): Abs(); if (opCycle == 4) { operand = mmu->CpuReadByte(workingAddr);  BIT(); } break; //BIT, abs

    case(0x9D): Abx(); if (opCycle == 5) { STA(); } break;                                                                                                    //STA, abx
    case(0x1D): Abx(); if (opCycle == 4 && !crossedPageBoundary) { ORA(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); ORA(); } break; //ORA, abx
    case(0x3D): Abx(); if (opCycle == 4 && !crossedPageBoundary) { AND(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); AND(); } break; //AND, abx
    case(0x5D): Abx(); if (opCycle == 4 && !crossedPageBoundary) { EOR(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); EOR(); } break; //EOR, abx
    case(0x7D): Abx(); if (opCycle == 4 && !crossedPageBoundary) { ADC(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); ADC(); } break; //ADC, abx
    case(0xBD): Abx(); if (opCycle == 4 && !crossedPageBoundary) { LDA(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); LDA(); } break; //LDA, abx
    case(0xDD): Abx(); if (opCycle == 4 && !crossedPageBoundary) { CMP(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); CMP(); } break; //CMP, abx
    case(0xFD): Abx(); if (opCycle == 4 && !crossedPageBoundary) { SBC(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); SBC(); } break; //SBC, abx
    case(0xBC): Abx(); if (opCycle == 4 && !crossedPageBoundary) { LDY(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); LDY(); } break; //LDY, abx
     

    case(0x99): Aby(); if (opCycle == 5) { STA(); } break;                                                                                                    //STA, aby 
    case(0x19): Aby(); if (opCycle == 4 && !crossedPageBoundary) { ORA(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); ORA(); } break; //ORA, aby    
    case(0x39): Aby(); if (opCycle == 4 && !crossedPageBoundary) { AND(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); AND(); } break; //AND, aby    
    case(0x59): Aby(); if (opCycle == 4 && !crossedPageBoundary) { EOR(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); EOR(); } break; //EOR, aby    
    case(0x79): Aby(); if (opCycle == 4 && !crossedPageBoundary) { ADC(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); ADC(); } break; //ADC, aby   
    case(0xB9): Aby(); if (opCycle == 4 && !crossedPageBoundary) { LDA(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); LDA(); } break; //LDA, aby    
    case(0xD9): Aby(); if (opCycle == 4 && !crossedPageBoundary) { CMP(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); CMP(); } break; //CMP, aby    
    case(0xF9): Aby(); if (opCycle == 4 && !crossedPageBoundary) { SBC(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); SBC(); } break; //SBC, aby
    case(0xBE): Aby(); if (opCycle == 4 && !crossedPageBoundary) { LDX(); } else if (opCycle == 5) { operand = mmu->CpuReadByte(workingAddr); LDX(); } break; //LDX, aby

    case(0x0E): Abs(); ASL(); break; //ASL, abs
    case(0x2E): Abs(); ROL(); break; //ROL, abs
    case(0x4E): Abs(); LSR(); break; //LSR, abs
    case(0x6E): Abs(); ROR(); break; //ROR, abs
    case(0xCE): Abs(); DEC(); break; //DEC, abs
    case(0xEE): Abs(); INC(); break; //INC, abs
    
    
    case(0x1E): Abx(); ASL(); break; //ASL, abx    
    case(0x3E): Abx(); ROL(); break; //ROL, abx    
    case(0x5E): Abx(); LSR(); break; //LSR, abx    
    case(0x7E): Abx(); ROR(); break; //ROR, abx           
    case(0xDE): Abx(); DEC(); break; //DEC, abx    
    case(0xFE): Abx(); INC(); break; //INC, abx


    default: //illegal opcodes. halt, catch fire.
        LOG_ERROR("Invalid opcode: {}", op);
        exit(-1);
    }

    opCycle++;

    //log post-op

    return 0;
}
