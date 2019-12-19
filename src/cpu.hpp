#ifndef __CPU_H__
#define __CPU_H__

#pragma once

#include <string>
#include "types.hpp"

class Cpu {
private:
  /* Constants */
  const u8 flagCvalue = 0x01;
  const u8 flagZvalue = 0x02;
  const u8 flagIvalue = 0x04;
  const u8 flagDvalue = 0x08;
  const u8 flagBvalue = 0x10;
  const u8 flagUvalue = 0x20;
  const u8 flagVvalue = 0x40;
  const u8 flagNvalue = 0x80;

  const u8 header     = 16;

  /* Memory */
  u8 ram[0x10000];    // 64KB of memory with addresses from 0x0000 to 0xFFFF

  /* Registers */
  s32 cycleCount; // Clock cycles
  u16 PC;         // Program counter
  u8  SP;         // Stack pointer
  u8  A;          // Accumulator
  u8  X;          // Index register X
  u8  Y;          // Index register Y

  /* Processor status S */
  u8 status;
  bool C;         // Carry flag        - bit 0
  bool Z;         // Zero flag         - bit 1
  bool I;         // Interrupt disable - bit 2
  bool D;         // Decimal mode      - bit 3
  bool B;         // Break command     - bit 4
  bool U;         // Unused flag       - bit 5
  bool V;         // Overflow flag     - bit 6
  bool N;         // Negative flag     - bit 7

  /* Interrupts */
  bool NMI;       // Non-Maskable interrupt
  bool IRQ;       // Maskable interrupt

public:
  Cpu();
  ~Cpu();

  void LoadRom             (std::string romFile = "DEFAULT_FILE");

  void NextOpcode          ();
  void ProcessOpcode       (u16 opcode);

private:
  void Reset               ();

  void SetStatus           ();
  void SetStatus           (u8 newStatus);
  void SetNMI              (bool value);
  void SetIRQ              (bool value);

  /* Addressing modes */
  u8  ZeroPage             (s32 cycles);
  u8  ZeroPageX            (s32 cycles);
  u8  ZeroPageY            (s32 cycles);
  u16 Absolute             (s32 cycles);
  u16 AbsoluteX            (s32 cycles, s32 extraCyclesForCrossedPage = 0);
  u16 AbsoluteY            (s32 cycles, s32 extraCyclesForCrossedPage = 0);
  u16 Indirect             (s32 cycles);
  u16 IndirectXPreIndexing (s32 cycles);
  u16 IndirectYPostIndexing(s32 cycles, s32 extraCyclesForCrossedPage = 0);
  u16 Immediate            (s32 cycles);

  /* Control Instructions -> From excel. Format: Mnemonic_Opcode */ 
  /* Load/Store Operations: Load a register from memory or stores the contents of a register to memory. */
  void LDA_A9(); /* Load Accumulator with Memory */
  void LDA_AD(); /* Load Accumulator with Memory */
  void LDA_BD(); /* Load Accumulator with Memory */
  void LDA_B9(); /* Load Accumulator with Memory */
  void LDA_A5(); /* Load Accumulator with Memory */
  void LDA_B5(); /* Load Accumulator with Memory */
  void LDA_A1(); /* Load Accumulator with Memory */
  void LDA_B1(); /* Load Accumulator with Memory */
  void LDX_A2(); /* Load Index X with Memory */
  void LDX_AE(); /* Load Index X with Memory */
  void LDX_BE(); /* Load Index X with Memory */
  void LDX_A6(); /* Load Index X with Memory */
  void LDX_B6(); /* Load Index X with Memory */
  void LDY_A0(); /* Load Index Y with Memory */
  void LDY_AC(); /* Load Index Y with Memory */
  void LDY_BC(); /* Load Index Y with Memory */
  void LDY_A4(); /* Load Index Y with Memory */
  void LDY_B4(); /* Load Index Y with Memory */
  void STA_8D(); /* Store Accumulator in Memory */
  void STA_9D(); /* Store Accumulator in Memory */
  void STA_99(); /* Store Accumulator in Memory */
  void STA_85(); /* Store Accumulator in Memory */
  void STA_95(); /* Store Accumulator in Memory */
  void STA_81(); /* Store Accumulator in Memory */
  void STA_91(); /* Store Accumulator in Memory */
  void STX_8E(); /* Store Index X in Memory */
  void STX_86(); /* Store Index X in Memory */
  void STX_96(); /* Store Index X in Memory */
  void STY_8C(); /* Store Index Y in Memory */
  void STY_84(); /* Store Index Y in Memory */
  void STY_94(); /* Store Index Y in Memory */
    
  /* Register Transfer Operations: Copy contents of X or Y register to the accumulator or copy contents of accumulator to X or Y register. */
  void TAX_AA(); /* Transfer Accumulator to Index X */
  void TAY_A8(); /* Transfer Accumulator to Index Y */
  void TXA_8A(); /* Transfer Index X to Accumulator */
  void TYA_98(); /* Transfer Index Y to Accumulator */

  /* Stack Operations: Push or pull the stack or manipulate stack pointer using X register. */
  void PHA_48(); /* Push Accumulator on Stack */
  void PHP_08(); /* Push Processor Status on Stack */
  void PLA_68(); /* Pull Accumulator from Stack */
  void PLP_28(); /* Pull Processor Status from Stack */
  void TSX_BA(); /* Transfer Stack Pointer to Index X */
  void TXS_9A(); /* Transfer Index X to Stack Pointer */

  /* Logical Operations: Perform logical operations on the accumulator and a value stored in memory. */
  void AND_29(); /* AND Memory with Accumulator */
  void AND_2D(); /* AND Memory with Accumulator */
  void AND_3D(); /* AND Memory with Accumulator */
  void AND_39(); /* AND Memory with Accumulator */
  void AND_25(); /* AND Memory with Accumulator */
  void AND_35(); /* AND Memory with Accumulator */
  void AND_21(); /* AND Memory with Accumulator */
  void AND_31(); /* AND Memory with Accumulator */
  void EOR_49(); /* Exclusive Or Memory with Accumulator */
  void EOR_4D(); /* Exclusive Or Memory with Accumulator */
  void EOR_5D(); /* Exclusive Or Memory with Accumulator */
  void EOR_59(); /* Exclusive Or Memory with Accumulator */
  void EOR_45(); /* Exclusive Or Memory with Accumulator */
  void EOR_55(); /* Exclusive Or Memory with Accumulator */
  void EOR_41(); /* Exclusive Or Memory with Accumulator */
  void EOR_51(); /* Exclusive Or Memory with Accumulator */
  void ORA_09(); /* OR Memory with Accumulator */
  void ORA_0D(); /* OR Memory with Accumulator */
  void ORA_1D(); /* OR Memory with Accumulator */
  void ORA_19(); /* OR Memory with Accumulator */
  void ORA_05(); /* OR Memory with Accumulator */
  void ORA_15(); /* OR Memory with Accumulator */
  void ORA_01(); /* OR Memory with Accumulator */
  void ORA_11(); /* OR Memory with Accumulator */    
  void BIT_2C(); /* Test Bits in Memory with Accumulator */
  void BIT_24(); /* Test Bits in Memory with Accumulator */

  /* Arithmetic Operations: Perform arithmetic operations on registers and memory. */
  void ROL_2A(); /* Rotate One Bit Left (Memory or Accumulator) */
  void ROL_2E(); /* Rotate One Bit Left (Memory or Accumulator) */
  void ROL_3E(); /* Rotate One Bit Left (Memory or Accumulator) */
  void ROL_26(); /* Rotate One Bit Left (Memory or Accumulator) */
  void ROL_36(); /* Rotate One Bit Left (Memory or Accumulator) */
  void ROR_6A(); /* Rotate One Bit Right (Memory or Accumulator) */
  void ROR_6E(); /* Rotate One Bit Right (Memory or Accumulator) */
  void ROR_7E(); /* Rotate One Bit Right (Memory or Accumulator) */
  void ROR_66(); /* Rotate One Bit Right (Memory or Accumulator) */
  void ROR_76(); /* Rotate One Bit Right (Memory or Accumulator) */
  void ADC_69(); /* Add Memory to Accumulator with Carry */
  void ADC_6D(); /* Add Memory to Accumulator with Carry */
  void ADC_7D(); /* Add Memory to Accumulator with Carry */
  void ADC_79(); /* Add Memory to Accumulator with Carry */
  void ADC_65(); /* Add Memory to Accumulator with Carry */
  void ADC_75(); /* Add Memory to Accumulator with Carry */
  void ADC_61(); /* Add Memory to Accumulator with Carry */
  void ADC_71(); /* Add Memory to Accumulator with Carry */
  void CMP_C9(); /* Compare Memory and Accumulator */
  void CMP_CD(); /* Compare Memory and Accumulator */
  void CMP_DD(); /* Compare Memory and Accumulator */
  void CMP_D9(); /* Compare Memory and Accumulator */
  void CMP_C5(); /* Compare Memory and Accumulator */
  void CMP_D5(); /* Compare Memory and Accumulator */
  void CMP_C1(); /* Compare Memory and Accumulator */
  void CMP_D1(); /* Compare Memory and Accumulator */
  void CPX_E0(); /* Compare Memory and Index X */
  void CPX_EC(); /* Compare Memory and Index X */
  void CPX_E4(); /* Compare Memory and Index X */
  void CPY_C0(); /* Compare Memory and Index Y */
  void CPY_CC(); /* Compare Memory and Index Y */
  void CPY_C4(); /* Compare Memory and Index Y */
  void SBC_E9(); /* Substract Memory from Accumulator with Borrow */
  void SBC_ED(); /* Substract Memory from Accumulator with Borrow */
  void SBC_FD(); /* Substract Memory from Accumulator with Borrow */
  void SBC_F9(); /* Substract Memory from Accumulator with Borrow */
  void SBC_E5(); /* Substract Memory from Accumulator with Borrow */    
  void SBC_F5(); /* Substract Memory from Accumulator with Borrow */
  void SBC_E1(); /* Substract Memory from Accumulator with Borrow */
  void SBC_F1(); /* Substract Memory from Accumulator with Borrow */

  /* Increments/Decrements: Increment or decrement the X or Y registers or a value stored in memory. */
  void DEC_CE(); /* Decrement Memory by One */
  void DEC_DE(); /* Decrement Memory by One */
  void DEC_C6(); /* Decrement Memory by One */
  void DEC_D6(); /* Decrement Memory by One */ 
  void DEX_CA(); /* Decrement Index X by One */
  void DEY_88(); /* Decrement Index Y by One */
  void INC_EE(); /* Increment Memory by One */
  void INC_FE(); /* Increment Memory by One */
  void INC_E6(); /* Increment Memory by One */
  void INC_F6(); /* Increment Memory by One */
  void INX_E8(); /* Increment Index X by One */
  void INY_C8(); /* Increment Index Y by One */

  /* Shifts: Shift the bits of either the accumulator or a memory location one bit to the left or right. */
  void ASL_0A(); /* Shift One Bit Left (Memory or Accummulator) */
  void ASL_0E(); /* Shift One Bit Left (Memory or Accummulator) */
  void ASL_1E(); /* Shift One Bit Left (Memory or Accummulator) */
  void ASL_06(); /* Shift One Bit Left (Memory or Accummulator) */
  void ASL_16(); /* Shift One Bit Left (Memory or Accummulator) */
  void LSR_4A(); /* Shift One Bit Right (Memory or Accumulator) */
  void LSR_4E(); /* Shift One Bit Right (Memory or Accumulator) */
  void LSR_5E(); /* Shift One Bit Right (Memory or Accumulator) */
  void LSR_46(); /* Shift One Bit Right (Memory or Accumulator) */
  void LSR_56(); /* Shift One Bit Right (Memory or Accumulator) */

  /* Jumps/Calls: Break sequential execution sequence, resuming from a specified address. */
  void RTS_60(); /* Return from  Subroutine */
  void JMP_4C(); /* Jump to New Location */
  void JMP_6C(); /* Jump to New Location */
  void JSR_20(); /* Jump to New Location Saving Return Address */

  /* Branches: Break sequential execution sequence, resuming from a specified address, if a condition is met. The condition involves examining a specific bit in the status register.*/
  void BCC_90(); /* Branch on Carry Clear */
  void BCS_B0(); /* Branch on Carry Set */
  void BEQ_F0(); /* Branch on Result Zero*/
  void BMI_30(); /* Branch on Result Minus*/
  void BNE_D0(); /* Branch on Result not Zero*/
  void BPL_10(); /* Branch on Result Plus */
  void BVC_50(); /* Branch on Overflow Clear*/
  void BVS_70(); /* Branch on Overflow Set */

  /* Status Register Operations: Set or clear a flag in the status register. */
  void CLC_18(); /* Clear Carry Flag */
  void CLD_D8(); /* Clear Decimal Mode */
  void CLI_58(); /* Clear interrupt Disable Bit */
  void CLV_B8(); /* Clear Overflow Flag */
  void SEC_38(); /* Set Carry Flag */
  void SED_F8(); /* Set Decimal Mode */
  void SEI_78(); /* Set interrupt Disable Bit*/

  /* System Functions: Perform rarely used functions. */
  void NOP_EA(); /* No Operation */
  void RTI_40(); /* Return from interrupt */
  void BRK_00(); /* Force Break */


  /* Imp: Implied */
  /* Acc: Accumulator */
  /* Imm: Immediate */
  /* Abs: Absolute */
  /* AbsX: Absolute with X Indexing */
  /* AbsY: Absolute with Y Indexing */
  /* JmpInd: Indirect Absolute */
  /* DP: Zero/Direct/Base Page */    
  /* DPX: Zero/Direct/Base Page with X Indexing */
  /* DPY: Zero/Direct/Base Page with Y Indexing */
  /* DPIndX: Zero/Direct/Base Page Indirect with X Pre-Indexing */
  /* DPIndY: Zero/Direct/Base Page Indirect with X Post-Indexing */
  /* Rel: Relative */
};

#endif //__CPU_H__