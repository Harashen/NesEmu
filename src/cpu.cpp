#include "cpu.hpp"
#pragma warning(disable:4996) // Disable error when using fopen

Cpu::Cpu()
{
  Reset();
}

Cpu::~Cpu()
{

}

// ToDo Review this method, done really fast to testing test rom.
void Cpu::LoadRom(std::string romFile)
{
  FILE *pRom = nullptr;
  u16   romHeader[16]; // ToDo Move to a memory class
  u16   ppuMemory[0x10000]; // ToDo Move to ppu class. 64KB of memory with addresses from 0x0000 to 0xFFFF]

  pRom = fopen(romFile.c_str(), "rb");

  fread(&romHeader[0], 1, 16, pRom);

  // Load Rom Header
  s16 prgRomBanks     = romHeader[4]; // Count of 16KB rom banks
  s16 chrRomBanks     = romHeader[5]; // Count of 8KB chr-rom/vrom banks 
  s16 romControlByte1 = romHeader[6]; // Bit dependant
  s16 romControlByte2 = romHeader[7]; //
  
  // Get Rom Size
  fseek(pRom, 0, SEEK_END);

  u32 romSize = ftell(pRom);

  rewind(pRom);

  // Create Rom Memory
  u8 *pRomMemory = new u8[romSize];

  memset(pRomMemory, 0, sizeof(pRomMemory));
  memset(ram       , 0, sizeof(ram       ));
  memset(ppuMemory , 0, sizeof(ppuMemory ));

  cycleCount = 0;

  // Seek Header (16) ???
  u16 bytesToRead = (prgRomBanks * 0x4000) + (chrRomBanks * 0x2000) + header;

  // Load everything from the Rom file to Rom memory
  size_t result   = fread(pRomMemory, 1 , bytesToRead, pRom);

  // Load Program Rom
  memcpy(&ram[0x8000], pRomMemory + header, 0x4000 * 2); // Copy 16KB * 2 banks
  //memcpy(&ram[0xC000], pRomMemory + header, 0x4000 * 2); // Copy 16KB * 2 banks

  // Load Video Rom 
  memcpy(&ppuMemory[0x0000], pRomMemory + 0x4000 + header, 0x2000); // Copy 8KB
  
  // Generate initial PC value
  u16 LL = ram[0xFFFC]; // low byte
  u16 HH = ram[0xFFFD]; // high byte
  HH <<= 8;
  u16 initialPC = HH | LL;

  initialPC = 0xC000; // To test nestest rom

  PC = initialPC;
}

void Cpu::NextOpcode()
{
  SetStatus();

  // ToDo Fix all this code, done really fast to testing test rom.
  u16 opcode = ram[PC] & 0x00FF;

  printf("PC -%X- Opcode: %X\n", PC, opcode);

  ProcessOpcode(opcode);
}

void Cpu::ProcessOpcode(u16 opcode)
{
  switch (opcode)
  {
    /* Load/Store Operations: Load a register from memory or stores the contents of a register to memory. */
    case 0xA9: LDA_A9(); break; 
    case 0xAD: LDA_AD(); break; 
    case 0xBD: LDA_BD(); break; 
    case 0xB9: LDA_B9(); break; 
    case 0xA5: LDA_A5(); break; 
    case 0xB5: LDA_B5(); break; 
    case 0xA1: LDA_A1(); break; 
    case 0xB1: LDA_B1(); break; 
    case 0xA2: LDX_A2(); break; 
    case 0xAE: LDX_AE(); break; 
    case 0xBE: LDX_BE(); break; 
    case 0xA6: LDX_A6(); break; 
    case 0xB6: LDX_B6(); break; 
    case 0xA0: LDY_A0(); break; 
    case 0xAC: LDY_AC(); break; 
    case 0xBC: LDY_BC(); break; 
    case 0xA4: LDY_A4(); break; 
    case 0xB4: LDY_B4(); break; 
    case 0x8D: STA_8D(); break; 
    case 0x9D: STA_9D(); break; 
    case 0x99: STA_99(); break; 
    case 0x85: STA_85(); break; 
    case 0x95: STA_95(); break; 
    case 0x81: STA_81(); break; 
    case 0x91: STA_91(); break; 
    case 0x8E: STX_8E(); break; 
    case 0x86: STX_86(); break; 
    case 0x96: STX_96(); break; 
    case 0x8C: STY_8C(); break; 
    case 0x84: STY_84(); break; 
    case 0x94: STY_94(); break; 

    /* Register Transfer Operations: Copy contents of X or Y register to the accumulator or copy contents of accumulator to X or Y register. */
    case 0xAA: TAX_AA(); break;
    case 0xA8: TAY_A8(); break;
    case 0x8A: TXA_8A(); break;
    case 0x98: TYA_98(); break;

    /* Stack Operations: Push or pull the stack or manipulate stack pointer using X register. */
    case 0x48: PHA_48(); break;
    case 0x08: PHP_08(); break;
    case 0x68: PLA_68(); break;
    case 0x28: PLP_28(); break;
    case 0xBA: TSX_BA(); break;
    case 0x9A: TXS_9A(); break;

    /* Logical Operations: Perform logical operations on the accumulator and a value stored in memory. */
    case 0x29: AND_29(); break;
    case 0x2D: AND_2D(); break;
    case 0x3D: AND_3D(); break;
    case 0x39: AND_39(); break;
    case 0x25: AND_25(); break;
    case 0x35: AND_35(); break;
    case 0x21: AND_21(); break;
    case 0x31: AND_31(); break;
    case 0x49: EOR_49(); break;
    case 0x4D: EOR_4D(); break;
    case 0x5D: EOR_5D(); break;
    case 0x59: EOR_59(); break;
    case 0x45: EOR_45(); break;
    case 0x55: EOR_55(); break;
    case 0x41: EOR_41(); break;
    case 0x51: EOR_51(); break;
    case 0x09: ORA_09(); break;
    case 0x0D: ORA_0D(); break;
    case 0x1D: ORA_1D(); break;
    case 0x19: ORA_19(); break;
    case 0x05: ORA_05(); break;
    case 0x15: ORA_15(); break;
    case 0x01: ORA_01(); break;
    case 0x11: ORA_11(); break;
    case 0x2C: BIT_2C(); break;
    case 0x24: BIT_24(); break;

    /* Arithmetic Operations: Perform arithmetic operations on registers and memory. */
    case 0x2A: ROL_2A(); break;
    case 0x2E: ROL_2E(); break;
    case 0x3E: ROL_3E(); break;
    case 0x26: ROL_26(); break;
    case 0x36: ROL_36(); break;
    case 0x6A: ROR_6A(); break;
    case 0x6E: ROR_6E(); break;
    case 0x7E: ROR_7E(); break;
    case 0x66: ROR_66(); break;
    case 0x76: ROR_76(); break;
    case 0x69: ADC_69(); break;
    case 0x6D: ADC_6D(); break;
    case 0x7D: ADC_7D(); break;
    case 0x79: ADC_79(); break;
    case 0x65: ADC_65(); break;
    case 0x75: ADC_75(); break;
    case 0x61: ADC_61(); break;
    case 0x71: ADC_71(); break;
    case 0xC9: CMP_C9(); break;
    case 0xCD: CMP_CD(); break;
    case 0xDD: CMP_DD(); break;
    case 0xD9: CMP_D9(); break;
    case 0xC5: CMP_C5(); break;
    case 0xD5: CMP_D5(); break;
    case 0xC1: CMP_C1(); break;
    case 0xD1: CMP_D1(); break;
    case 0xE0: CPX_E0(); break;
    case 0xEC: CPX_EC(); break;
    case 0xE4: CPX_E4(); break;
    case 0xC0: CPY_C0(); break;
    case 0xCC: CPY_CC(); break;
    case 0xC4: CPY_C4(); break;
    case 0xE9: SBC_E9(); break;
    case 0xED: SBC_ED(); break;
    case 0xFD: SBC_FD(); break;
    case 0xF9: SBC_F9(); break;
    case 0xE5: SBC_E5(); break;   
    case 0xF5: SBC_F5(); break;
    case 0xE1: SBC_E1(); break;
    case 0xF1: SBC_F1(); break;

    /* Increments/Decrements: Increment or decrement the X or Y registers or a value stored in memory. */
    case 0xCE: DEC_CE(); break;
    case 0xDE: DEC_DE(); break;
    case 0xC6: DEC_C6(); break;
    case 0xD6: DEC_D6(); break;
    case 0xCA: DEX_CA(); break;
    case 0x88: DEY_88(); break;
    case 0xEE: INC_EE(); break;
    case 0xFE: INC_FE(); break;
    case 0xE6: INC_E6(); break;
    case 0xF6: INC_F6(); break;
    case 0xE8: INX_E8(); break;
    case 0xC8: INY_C8(); break;

    /* Shifts: Shift the bits of either the accumulator or a memory location one bit to the left or right. */
    case 0x0A: ASL_0A(); break;
    case 0x0E: ASL_0E(); break;
    case 0x1E: ASL_1E(); break;
    case 0x06: ASL_06(); break;
    case 0x16: ASL_16(); break;
    case 0x4A: LSR_4A(); break;
    case 0x4E: LSR_4E(); break;
    case 0x5E: LSR_5E(); break;
    case 0x46: LSR_46(); break;
    case 0x56: LSR_56(); break;

    /* Jumps/Calls: Break sequential execution sequence, resuming from a specified address. */
    case 0x60: RTS_60(); break;
    case 0x4C: JMP_4C(); break;
    case 0x6C: JMP_6C(); break;
    case 0x20: JSR_20(); break;

    /* Branches: Break sequential execution sequence, resuming from a specified address, if a condition is met. The condition involves examining a specific bit in the status register.*/
    case 0x90: BCC_90(); break;
    case 0xB0: BCS_B0(); break;
    case 0xF0: BEQ_F0(); break;
    case 0x30: BMI_30(); break;
    case 0xD0: BNE_D0(); break;
    case 0x10: BPL_10(); break;
    case 0x50: BVC_50(); break;
    case 0x70: BVS_70(); break;

    /* Status Register Operations: Set or clear a flag in the status register. */
    case 0x18: CLC_18(); break;
    case 0xD8: CLD_D8(); break;
    case 0x58: CLI_58(); break;
    case 0xB8: CLV_B8(); break;
    case 0x38: SEC_38(); break;
    case 0xF8: SED_F8(); break;
    case 0x78: SEI_78(); break;

    /* System Functions: Perform rarely used functions. */
    case 0xEA: NOP_EA(); break;
    case 0x40: RTI_40(); break;
    case 0x00: BRK_00(); break;
  }
}

void Cpu::Reset()
{
  /* Reset flags */
  C = false; // Bit 0
  Z = false; // Bit 1
  I = false; // Bit 2
  D = false; // Bit 3
  B = false; // Bit 4
  U = true;  // Bit 5
  V = false; // Bit 6
  N = false; // Bit 7

  /* Reset registers */
  A = 0;
  X = 0;
  Y = 0;

  SP = 0xFD; // Initial memory for Stack Pos32er
}

void Cpu::SetStatus()
{
  status = 0;

  if (C) status |= flagCvalue; // Bit 0
  if (Z) status |= flagZvalue; // Bit 1
  if (I) status |= flagIvalue; // Bit 2
  if (D) status |= flagDvalue; // Bit 3
  if (B) status |= flagBvalue; // Bit 4
  if (U) status |= flagUvalue; // Bit 5
  if (V) status |= flagVvalue; // Bit 6
  if (N) status |= flagNvalue; // Bit 7
}

void Cpu::SetStatus(u8 newStatus)
{
  C = (newStatus & flagCvalue) == flagCvalue;
  Z = (newStatus & flagZvalue) == flagZvalue;
  I = (newStatus & flagIvalue) == flagIvalue;
  D = (newStatus & flagDvalue) == flagDvalue;
  B = (newStatus & flagBvalue) == flagBvalue;
  U = (newStatus & flagUvalue) == flagUvalue;
  V = (newStatus & flagVvalue) == flagVvalue;
  N = (newStatus & flagNvalue) == flagNvalue;

  SetStatus();
}

void Cpu::SetNMI(bool value)
{
  NMI = value;
}

void Cpu::SetIRQ(bool value)
{
  IRQ = value;
}

/* Addressing modes */
u8 Cpu::ZeroPage(s32 cycles)
{
  u8 result = ram[PC + 1] + X;

  cycleCount += cycles;
  PC         += 2;

  return result;
}

u8 Cpu::ZeroPageX(s32 cycles)
{
  u8 result = ram[PC + 1];

  cycleCount += cycles;
  PC         += 2;

  return result;
}

u8 Cpu::ZeroPageY(s32 cycles)
{
  u8 result = ram[PC + 1] + Y;

  cycleCount += cycles;
  PC         += 2;

  return result;
}

u16 Cpu::Absolute(s32 cycles)
{
  u16 LL = ram[PC + 1]; // low byte
  u16 HH = ram[PC + 2]; // high byte
  HH <<= 8;
  u16 result = HH | LL;

  cycleCount += cycles;
  PC         += 3;

  return result;
}

u16 Cpu::AbsoluteX(s32 cycles, s32 extraCycles)
{
  u16 LL = ram[PC + 1]; // low byte
  u16 HH = ram[PC + 2]; // high byte
  HH <<= 8;
  u16 result = (HH | LL) + X;

  // Check if it was page cross
  if (extraCycles) 
  {
    if (LL + X <= 255)
      extraCycles = 0;
  }

  cycleCount += cycles + extraCycles;
  PC         += 3;

  return result;
}

u16 Cpu::AbsoluteY(s32 cycles, s32 extraCycles)
{
  u16 LL = ram[PC + 1]; // low byte
  u16 HH = ram[PC + 2]; // high byte
  HH <<= 8;
  u16 result = (HH | LL) + Y;

  // Check if it is page cross
  if (extraCycles)
  {
    if (LL + Y <= 255)
      extraCycles = 0;
  }

  cycleCount += cycles + extraCycles;
  PC         += 3;

  return result;
}

u16 Cpu::Indirect(s32 cycles)
{
  u16 LL = ram[PC + 1]; // low byte
  u16 HH = ram[PC + 2]; // high byte
  HH <<= 8;
  u16 address = (HH | LL);

  u16 XX = ram[address];
  u16 YY;

  // Check if memory is out of bounds
  if (address + 1 > 0x02FF)
  {
    address -= 0x00FF;
    YY = ram[address];
  }
  else
  {
    YY = ram[address + 1];
  }

  YY <<= 8;

  u16 result = (YY | XX);

  cycleCount += cycles;
  PC         += 3;

  return result;
}

u16 Cpu::IndirectXPreIndexing(s32 cycles)
{
  u16 BB = ram[PC + 1]; // low byte
  u16 XX = ram[BB + X];
  u16 YY = ram[BB + X + 1];

  YY <<= 8;

  u16 result = (YY | XX);

  cycleCount += cycles;
  PC         += 2;

  return result;
}

u16 Cpu::IndirectYPostIndexing(s32 cycles, s32 extraCycles)
{
  u16 BB = ram[PC + 1]; // low byte    
  u16 XX = ram[BB];
  u16 YY = ram[BB + 1];

  YY <<= 8;

  u16 result = (YY | XX) + Y;

  // Check if it is page cross
  if (extraCycles)
  {
    if (XX + Y <= 255)
      extraCycles = 0;
  }

  cycleCount += cycles + extraCycles;
  PC         += 2;

  return result;
}

u16 Cpu::Immediate(s32 cycles)
{
  u16 result = PC + 1;

  cycleCount += cycles;
  PC         += 2;

  return result;
}

/* Load/Store Operations: Load a register from memory or stores the contents of a register to memory. */
void Cpu::LDA_A9() // Immediate, 2 cycles
{
  A = ram[Immediate(2)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_AD() // Absolute , 4 cycles
{
  A = ram[Absolute(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_BD() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{
  A = ram[AbsoluteX(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_B9() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  A = ram[AbsoluteY(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_A5() // Zero page, 3 cycles
{
  A = ram[ZeroPage(3)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_B5() // Zero page X, 4 cycles
{
  A = ram[ZeroPageX(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_A1() // Indirect X Pre-Indexing, 6 cycles
{
  A = ram[IndirectXPreIndexing(6)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDA_B1() // Indirect Y Post-Indexing, 5 cycles
{
  A = ram[IndirectYPostIndexing(5)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::LDX_A2() // Immediate, 2 cycles
{
  X = ram[Immediate(2)];

  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;
}

void Cpu::LDX_AE() // Absolute , 4 cycles
{
  X = ram[Absolute(4)];

  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;
}

void Cpu::LDX_BE() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  X = ram[AbsoluteY(4, 1)];

  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;
}

void Cpu::LDX_A6() // Zero page, 3 cycles
{
  X = ram[ZeroPage(3)];

  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;
}

void Cpu::LDX_B6() // Zero page Y, 4 cycles
{
  X = ram[ZeroPageY(4)];

  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;
}

void Cpu::LDY_A0() // Immediate, 2 cycles
{
  Y = ram[Immediate(2)];

  // Set flags
  Z = Y == 0;
  N = (Y & 0x80) == 0x80;
}

void Cpu::LDY_AC() // Absolute , 4 cycles
{
  Y = ram[Absolute(4)];

  // Set flags
  Z = Y == 0;
  N = (Y & 0x80) == 0x80;
}

void Cpu::LDY_BC() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  Y = ram[AbsoluteX(4, 1)];

  // Set flags
  Z = Y == 0;
  N = (Y & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::LDY_A4() // Zero page, 3 cycles
{
  Y = ram[ZeroPage(3)];

  // Set flags
  Z = Y == 0;
  N = (Y & 0x80) == 0x80;
}

void Cpu::LDY_B4() // Zero page X, 4 cycles
{
  Y = ram[ZeroPageX(4)];

  // Set flags
  Z = Y == 0;
  N = (Y & 0x80) == 0x80;
}

void Cpu::STA_8D() // Absolute, 4 cycles
{
  u16 position = Absolute(4);

  ram[position] = A;
}

void Cpu::STA_9D() // Absolute X Indexing, 5 cycles
{
  u16 position = AbsoluteX(5);

  ram[position] = A;
}

void Cpu::STA_99() // Absolute Y Indexing, 5 cycles
{
  u16 position = AbsoluteY(5);

  ram[position] = A;
}

void Cpu::STA_85() // Zero page, 3 cycles
{
  u16 position = ZeroPage(3);

  ram[position] = A;
}

void Cpu::STA_95() // Zero page X, 4 cycles
{
  u16 position = ZeroPageX(4);

  ram[position] = A;
}

void Cpu::STA_81() // Indirect X Pre-Indexing, 6 cycles
{
  u16 position = IndirectXPreIndexing(6);

  ram[position] = A;
}

void Cpu::STA_91() // Indirect Y Post-Indexing, 6 cycles
{
  u16 position = IndirectYPostIndexing(6);

  ram[position] = A;
}

void Cpu::STX_8E() // Absolute, 4 cycles
{
  u16 position = Absolute(4);

  ram[position] = X;
}

void Cpu::STX_86() // Zero page, 3 cycles
{
  u16 position = ZeroPage(3);

  ram[position] = X;
}

void Cpu::STX_96() // Zero page Y, 4 cycles
{
  u16 position = ZeroPageY(4);

  ram[position] = X;
}

void Cpu::STY_8C() // Absolute, 4 cycles
{
  u16 position = Absolute(4);

  ram[position] = Y;
}

void Cpu::STY_84() // Zero page, 3 cycles
{
  u16 position = ZeroPage(3);

  ram[position] = Y;
}

void Cpu::STY_94() // Zero page X, 4 cycles
{
  u16 position = ZeroPageX(4);

  ram[position] = Y;
}

/* Register Transfer Operations: Copy contents of X or Y register to the accumulator or copy contents of accumulator to X or Y register. */
void Cpu::TAX_AA() // Implied, 2 cycles
{
  X = A;
    
  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;

  cycleCount += 2;
  ++PC;
}

void Cpu::TAY_A8() // Implied, 2 cycles
{
  Y = A;
    
  // Set flags
  Z = Y == 0;
  N = (Y & 0x80) == 0x80;

  cycleCount += 2;
  ++PC;
}

void Cpu::TXA_8A() // Implied, 2 cycles
{
  A = X;
    
  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  cycleCount += 2;
  ++PC;
}

void Cpu::TYA_98() // Implied, 2 cycles
{
  A = Y;

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  cycleCount += 2;
  ++PC;
}

/* Stack Operations: Push or pull the stack or manipulate stack pos32er using X register. */
void Cpu::PHA_48() // Implied, 3 cycles
{
  SP--; // Decrement before pushing on
  ram[0x101 + SP] = A;

  cycleCount += 3;
  ++PC;
}

void Cpu::PHP_08() // Implied, 3 cycles
{
  SP--; // Decrement before pushing on
  ram[0x101 + SP] = status;

  cycleCount += 3;
  ++PC;
}

void Cpu::PLA_68() // Implied, 4 cycles
{
  A = ram[0x101 + SP];
  SP++;  // Decrement after pulled off
    
  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  cycleCount += 4;
  ++PC;
}

void Cpu::PLP_28() // Implied, 4 cycles
{
  u8 newStatus = ram[0x101 + SP];
  SP++;  // Decrement after pulled off

  SetStatus(newStatus);

  cycleCount += 4;
  ++PC;
}

void Cpu::TSX_BA()  // Implied, 2 cycles
{
  X = SP; 

  // Set flags
  Z = X == 0;
  N = (X & 0x80) == 0x80;

  cycleCount += 2;
  ++PC;
}

void Cpu::TXS_9A() // Implied, 2 cycles
{
  SP = X;
    
  cycleCount += 2;
  ++PC;
}

/* Logical Operations: Perform logical operations on the accumulator and a value stored in memory. */
void Cpu::AND_29() // Immediate, 2 cycles
{
  A = A & ram[Immediate(2)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::AND_2D() // Absolute, 4 cycles
{
  A = A & ram[Absolute(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::AND_3D() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  A = A & ram[AbsoluteX(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::AND_39() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  A = A & ram[AbsoluteY(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::AND_25() // Zero page, 3 cycles
{
  A = A & ram[ZeroPage(3)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::AND_35() // Zero page X, 4 cycles
{
  A = A & ram[ZeroPageX(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::AND_21() // Indirect X Pre-Indexing, 6 cycles
{
  A = A & ram[IndirectXPreIndexing(6)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::AND_31() // Indirect Y Post-Indexing, 5 cycles
{
  A = A & ram[IndirectYPostIndexing(5)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::EOR_49() // Immediate, 2 cycles
{
  A = A ^ ram[Immediate(2)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::EOR_4D() // Absolute, 4 cycles
{
  A = A ^ ram[Absolute(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::EOR_5D() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  A = A ^ ram[AbsoluteX(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::EOR_59() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  A = A ^ ram[AbsoluteY(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::EOR_45() // Zero page, 3 cycles
{
  A = A ^ ram[ZeroPage(3)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::EOR_55() // Zero page X, 4 cycles
{
  A = A ^ ram[ZeroPageX(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::EOR_41() // Indirect X Pre-Indexing, 6 cycles
{
  A = A ^ ram[IndirectXPreIndexing(6)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::EOR_51() // Indirect Y Post-Indexing, 5 cycles, + 1 if a page is crossed
{
  A = A ^ ram[IndirectYPostIndexing(5, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::ORA_09() // Immediate, 2 cycles
{
  A = A | ram[Immediate(2)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::ORA_0D() // Absolute, 4 cycles
{
  A = A | ram[Absolute(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::ORA_1D() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  A = A | ram[AbsoluteX(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::ORA_19() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 previousState = PC & 0xFF00;
  A = A | ram[AbsoluteY(4, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;

  // If page crossed, 1 more cycle
  if (previousState != (PC & 0xFF00))
    cycleCount++;
}

void Cpu::ORA_05() // Zero page, 3 cycles
{
  A = A | ram[ZeroPage(3)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::ORA_15() // Zero page X, 4 cycles
{
  A = A | ram[ZeroPageX(4)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::ORA_01() // Indirect X Pre-Indexing, 6 cycles
{
  A = A | ram[IndirectXPreIndexing(6)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::ORA_11() // Indirect Y Post-Indexing, 5 cycles, + 1 if a page is crossed
{
  A = A | ram[IndirectYPostIndexing(5, 1)];

  // Set flags
  Z = A == 0;
  N = (A & 0x80) == 0x80;
}

void Cpu::BIT_2C() // Absolute, 4 cycles
{
  u8 ramPos = ram[Absolute(4)];
  u8 result = A | ramPos;

  // Set flags
  Z = result == 0;
  V = (ramPos & 0x40) == 0x40;
  N = (ramPos & 0x80) == 0x80;
}

void Cpu::BIT_24() // Zero page, 3 cycles
{
  u8 ramPos = ram[ZeroPage(3)];
  u8 result = A | ramPos;

  // Set flags
  Z = result == 0;
  V = (ramPos & 0x40) == 0x40;
  N = (ramPos & 0x80) == 0x80;
}

/* Arithmetic Operations: Perform arithmetic operations on registers and memory. */
void Cpu::ROL_2A() // Accumulator, 2 cycles
{
  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x80;
     
  // Shift 1 bit left
  A <<= 1;

  if (previousCarry)
    A |= 1 << 0;
  else
    A &= ~(1 << 0);

  N = previousCarry;
  Z = A == 0;

  cycleCount += 2;
  ++PC;
}

void Cpu::ROL_2E() // Absolute, 6 cycles
{
  u16 position = Absolute(6);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x80;

  // Shift 1 bit left
  memValue <<= 1;

  if (previousCarry)
    memValue |= 1 << 0;
  else
    memValue &= ~(1 << 0);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROL_3E() // Absolute X , 7 cycles
{
  u16 position = AbsoluteX(7);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x80;

  // Shift 1 bit left
  memValue <<= 1;

  if (previousCarry)
    memValue |= 1 << 0;
  else
    memValue &= ~(1 << 0);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROL_26() // Zero page, 5 cycles
{
  u16 position = ZeroPage(5);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x80;

  // Shift 1 bit left
  memValue <<= 1;

  if (previousCarry)
    memValue |= 1 << 0;
  else
    memValue &= ~(1 << 0);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROL_36() // Zero page X Indexing, 6 cycles
{
  u16 position = ZeroPageX(6);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x80;

  // Shift 1 bit left
  memValue <<= 1;

  if (previousCarry)
    memValue |= 1 << 0;
  else
    memValue &= ~(1 << 0);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROR_6A()// Accumulator, 2 cycles
{
  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x01;

  // Shift 1 bit left
  A >>= 1;

  if (previousCarry)
    A |= 1 << 7;
  else
    A &= ~(1 << 7);

  N = previousCarry;
  Z = A == 0;

  cycleCount += 2;
  ++PC;
}

void Cpu::ROR_6E() // Absolute, 6 cycles
{
  u16 position = Absolute(6);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x01;

  // Shift 1 bit left
  memValue >>= 1;

  if (previousCarry)
    memValue |= 1 << 7;
  else
    memValue &= ~(1 << 7);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROR_7E() // Absolute X, 7 cycles
{
  u16 position = AbsoluteX(7);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x01;

  // Shift 1 bit left
  memValue >>= 1;

  if (previousCarry)
    memValue |= 1 << 7;
  else
    memValue &= ~(1 << 7);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROR_66() // Zero page, 5 cycles
{
  u16 position = ZeroPage(5);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x01;

  // Shift 1 bit left
  memValue >>= 1;

  if (previousCarry)
    memValue |= 1 << 7;
  else
    memValue &= ~(1 << 7);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}

void Cpu::ROR_76() // Zero page X Indexing, 6 cycles
{
  u16 position = ZeroPageX(6);
  u8  memValue = ram[position];

  // Get previous carry flag
  bool previousCarry = C;

  C = A & 0x01;

  // Shift 1 bit left
  memValue >>= 1;

  if (previousCarry)
    memValue |= 1 << 7;
  else
    memValue &= ~(1 << 7);

  N = previousCarry;
  Z = memValue == 0;

  ram[position] = memValue;
}


void Cpu::ADC_69() // Immediate, 2 cycles
{
  u16 position  = Immediate(2);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_6D() // Absolute, 4 cycles
{
  u16 position  = Absolute(4);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_7D() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 position  = AbsoluteX(4, 1);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_79() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 position  = AbsoluteY(4, 1);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_65() // Zero page, 3 cycles
{
  u16 position  = ZeroPage(3);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_75() // Zero page X Indexing, 4 cycles
{
  u16 position = ZeroPageX(4);
  u8  memValue = ram[position];
  u8 previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_61() // Indirect X Pre-Indexing, 6 cycles
{
  u16 position  = IndirectXPreIndexing(6);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::ADC_71() // Indirect Y Post-Indexing, 5 cycles, + 1 if a page is crossed
{
  u16 position  = IndirectYPostIndexing(5, 1);
  u8  memValue  = ram[position];
  u8  previousA = A;
    
  A += memValue + C;

  V = (previousA ^ A) & (memValue ^ A) & 0x80; // Overflow check
  C = (previousA + memValue + C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}


void Cpu::CMP_C9() // Immediate, 2 cycles
{ 
  u16 position  = Immediate(2);
  u8  memValue  = ram[position];
  u8  result    = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CMP_CD() // Absolute, 4 cycles
{ 
  u16 position  = Absolute(4);
  u8  memValue  = ram[position];
  u8  result    = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CMP_DD() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{ 
  u16 pageBefore = PC & 0xFF00;
  u16 position   = AbsoluteX(4);
  u8  memValue   = ram[position];
  u8  result     = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;

  // Check if page was crossed
  if ((PC & 0xFF00) != pageBefore)
    ++cycleCount;
}

void Cpu::CMP_D9() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{
  u16 pageBefore = PC & 0xFF00;
  u16 position   = AbsoluteY(4);
  u8  memValue   = ram[position];
  u8  result     = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;

  // Check if page was crossed
  if ((PC & 0xFF00) != pageBefore)
    ++cycleCount;
}

void Cpu::CMP_C5() // Zero page, 3 cycles
{ 
  u16 position  = ZeroPage(3);
  u8  memValue  = ram[position];
  u8  result    = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CMP_D5() // Zero page X Indexing, 4 cycles
{ 
  u16 position  = ZeroPageX(4);
  u8  memValue  = ram[position];
  u8  result    = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CMP_C1() // Indirect X Pre-Indexing, 6 cycles
{  
  u16 position  = Indirect(6);
  u8  memValue  = ram[position];
  u8  result    = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CMP_D1() // Indirect Y Post-Indexing, 5 cycles, + 1 if a page is crossed
{
  u16 pageBefore = PC & 0xFF00;
  u16 position   = IndirectYPostIndexing(5);
  u8  memValue   = ram[position];
  u8  result     = A - memValue;

  C = A >= memValue;
  Z = A == memValue;
  N = (result & flagNvalue) == flagNvalue;

  // Check if page was crossed
  if ((PC & 0xFF00) != pageBefore)
    ++cycleCount;
}


void Cpu::CPX_E0()  // Immediate, 2 cycles
{  
  u16 position  = Immediate(2);
  u8  memValue  = ram[position];
  u8  result    = X - memValue;

  C = X >= memValue;
  Z = X == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CPX_EC() // Absolute, 4 cycles
{
  u16 position = Absolute(4);
  u8  memValue = ram[position];
  u8  result   = X - memValue;

  C = X >= memValue;
  Z = X == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CPX_E4() // Zero page, 3 cycles
{ 
  u16 position  = ZeroPage(3);
  u8  memValue  = ram[position];
  u8  result    = X - memValue;

  C = X >= memValue;
  Z = X == memValue;
  N = (result & flagNvalue) == flagNvalue;
}


void Cpu::CPY_C0()  // Immediate, 2 cycles
{  
  u16 position  = Immediate(2);
  u8  memValue  = ram[position];
  u8  result    = Y - memValue;

  C = Y >= memValue;
  Z = Y == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CPY_CC() // Absolute, 4 cycles
{ 
  u16 position = Absolute(4);
  u8  memValue = ram[position];
  u8  result   = Y - memValue;

  C = Y >= memValue;
  Z = Y == memValue;
  N = (result & flagNvalue) == flagNvalue;
}

void Cpu::CPY_C4() // Zero page, 3 cycles
{ 
  u16 position  = ZeroPage(3);
  u8  memValue  = ram[position];
  u8  result    = Y - memValue;

  C = Y >= memValue;
  Z = Y == memValue;
  N = (result & flagNvalue) == flagNvalue;
}


void Cpu::SBC_E9() // Immediate, 2 cycles
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = Immediate(2);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_ED() // Absolute, 4 cycles
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = Absolute(4);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_FD() // Absolute X Indexing, 4 cycles, + 1 if a page is crossed
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = AbsoluteX(4);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_F9() // Absolute Y Indexing, 4 cycles, + 1 if a page is crossed
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = AbsoluteY(4);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_E5() // Zero page, 3 cycles
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = ZeroPage(3);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_F5() // Zero page X Indexing, 4 cycles
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = ZeroPageX(4);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_E1() // Indirect X Pre-Indexing, 6 cycles
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = IndirectXPreIndexing(6);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}

void Cpu::SBC_F1() // Indirect Y Post-Indexing, 5 cycles, + 1 if a page is crossed
{ 
  // ToDo Test if it works (unsigned A) ???
  u16 position  = IndirectYPostIndexing(5);
  u8  memValue  = ram[position];
  u8  previousA = A;

  A -= memValue - !C;

  V = (previousA ^ A) & (~memValue ^ A) & 0x80; // Overflow check
  C = (previousA - memValue - !C) > 255; // Carry check
  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
}


void Cpu::DEC_CE() // Absolute, 6 cycles
{
  u16 position  = Absolute(6);
  u8  memValue  = ram[position] - 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::DEC_DE() // Absolute X Indexing, 7 cycles
{
  u16 position  = AbsoluteX(7);
  u8  memValue  = ram[position] - 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::DEC_C6() // Zero page, 5 cycles
{
  u16 position  = ZeroPage(5);
  u8  memValue  = ram[position] - 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::DEC_D6() // Zero page X Indexing, 6 cycles
{
  u16 position  = ZeroPageX(6);
  u8  memValue  = ram[position] - 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::DEX_CA() // Implied, 2 cycles
{
  --X;

  Z = X == 0;
  N = (X & flagNvalue) == flagNvalue;

  cycleCount += 2;
  ++PC;
}

void Cpu::DEY_88() // Implied, 2 cycles
{
  --Y;

  Z = Y == 0;
  N = (Y & flagNvalue) == flagNvalue;

  cycleCount += 2;
  ++PC;
}

void Cpu::INC_EE() // Absolute, 6 cycles
{
  u16 position  = Absolute(6);
  u8  memValue  = ram[position] + 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::INC_FE() // Absolute X Indexing, 7 cycles
{
  u16 position  = AbsoluteX(7);
  u8  memValue  = ram[position] + 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::INC_E6() // Zero page, 5 cycles
{
  u16 position  = ZeroPage(5);
  u8  memValue  = ram[position] + 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::INC_F6() // Zero page X Indexing, 6 cycles
{
  u16 position  = ZeroPageX(6);
  u8  memValue  = ram[position] + 1;

  ram[position] = memValue;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;
}

void Cpu::INX_E8() // Implied, 2 cycles
{
  ++X;

  Z = X == 0;
  N = (X & flagNvalue) == flagNvalue;

  cycleCount += 2;
  ++PC;
}

void Cpu::INY_C8() // Implied, 2 cycles
{
  ++Y;

  Z = X == 0;
  N = (X & flagNvalue) == flagNvalue;

  cycleCount += 2;
  ++PC;
} 

/* Shifts: Shift the bits of either the accumulator or a memory location one bit to the left or right. */
void Cpu::ASL_0A() // Accumulator, 2 cycles
{
  C = A & 0x80;

  A <<= 1;

  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;
  
  cycleCount += 2;
  ++PC;
}

void Cpu::ASL_0E() // Absolute, 6 cycles
{
  u16 position  = Absolute(6);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue <<= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::ASL_1E() // Absolute X Indexing, 7 cycles
{
  u16 position  = AbsoluteX(7);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue <<= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::ASL_06() // Zero page, 5 cycles
{
  u16 position  = ZeroPage(5);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue <<= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::ASL_16() // Zero page X Indexing, 6 cycles
{
  u16 position  = ZeroPageX(6);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue <<= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::LSR_4A() // Accumulator, 2 cycles
{
  C = A & 0x01;

  A >>= 1;

  Z = A == 0;
  N = (A & flagNvalue) == flagNvalue;

  cycleCount += 2;
  ++PC;
}

void Cpu::LSR_4E() // Absolute, 6 cycles
{
  u16 position  = Absolute(6);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue >>= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::LSR_5E() // Absolute X Indexing, 7 cycles 
{
  u16 position  = AbsoluteX(7);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue >>= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::LSR_46() // Zero page, 5 cycles
{
  u16 position  = ZeroPage(5);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue >>= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

void Cpu::LSR_56() // Zero page X Indexing, 6 cycles
{
  u16 position  = ZeroPageX(6);
  u8  memValue  = ram[position];

  C = memValue & 0x80;

  memValue >>= 1;

  Z = memValue == 0;
  N = (memValue & flagNvalue) == flagNvalue;

  ram[position] = memValue;
}

/* Jumps/Calls: Break sequential execution sequence, resuming from a specified address. */
void Cpu::RTS_60() // Implied, 6 cycles
{
  u16 LL = ram[0x101 + SP++]; // low byte
  u16 HH = ram[0x101 + SP++]; // high byte
  HH <<= 8;
  u16 memValue = HH | LL;

  PC = memValue + 1;

  cycleCount += 6;
}

void Cpu::JMP_4C() // Absolute, 3 cycles
{
  PC = Absolute(3);
}

void Cpu::JMP_6C() // Indirect, 5 cycles
{
  PC = Indirect(5);
}

void Cpu::JSR_20() // Absolute, 6 cycles
{
  u16 position = Absolute(6);
  u8  LL = (PC - 1) >> 8;   // low byte
  u8  HH = (PC - 1) & 0xFF; // high byte

  ram[0x101 + --SP] = LL;  
  ram[0x101 + --SP] = HH;

  PC = position;
} 

/* Branches: Break sequential execution sequence, resuming from a specified address, if a condition is met. The condition involves examining a specific bit in the status register.*/
void Cpu::BCC_90() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;
  
  if (C == false)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BCS_B0() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;

  if (C == true)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BEQ_F0() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;
  
  if (Z == true)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BMI_30() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;

  if (N == true)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BNE_D0() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;

  if (Z == false)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BPL_10() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;

  if (N == false)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BVC_50() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;

  if (V == false)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
}

void Cpu::BVS_70() // Relative, 2 cycles, +1 if branch succeeds and +2 if a page is crossed
{
  cycleCount += 2;
  PC         += 2;

  if (V == true)
  {  
    u16 pageBefore = PC & 0xFF00;
    u8  memValue   = ram[PC + 1];

    cycleCount += 1; // Branch succeeds
    PC         += memValue;

    // Check if page was crossed
    if ((PC & 0xFF00) != pageBefore)
      cycleCount += 2;
  }
} 

/* Status Register Operations: Set or clear a flag in the status register. */
void Cpu::CLC_18() // Implied, 2 cycles
{
  C = false;

  cycleCount += 2;
  ++PC;
}

void Cpu::CLD_D8() // Implied, 2 cycles
{
  D = false;

  cycleCount += 2;
  ++PC;
}

void Cpu::CLI_58() // Implied, 2 cycles
{
  I = false;

  cycleCount += 2;
  ++PC;
}

void Cpu::CLV_B8() // Implied, 2 cycles
{
  V = false;

  cycleCount += 2;
  ++PC;
}

void Cpu::SEC_38() // Implied, 2 cycles
{
  C = true;

  cycleCount += 2;
  ++PC;
}

void Cpu::SED_F8() // Implied, 2 cycles
{
  D = true;

  cycleCount += 2;
  ++PC;
}

void Cpu::SEI_78() // Implied, 2 cycles
{
  I = true;

  cycleCount += 2;
  ++PC;
} 

/* System Functions: Perform rarely used functions. */
void Cpu::NOP_EA() // Implied, 2 cycles
{
  cycleCount += 2;
  ++PC;
}

void Cpu::RTI_40() // Implied, 6 cycles
{
  u8 newStatus = ram[0x101 + SP++];

  SetStatus(newStatus);

  cycleCount += 2;

  u16 LL = ram[0x101 + SP++]; // low byte
  u16 HH = ram[0x101 + SP++]; // high byte
  HH <<= 8;
  u16 memValue = HH | LL;

  PC = memValue;
}

void Cpu::BRK_00() // Immediate, 7 cycles
{
  cycleCount += 7;

  system("pause");
}
