// InstructionFactoryARM.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#ifndef EDA_INSTRUCTIONFACTORYARM_H_
#define EDA_INSTRUCTIONFACTORYARM_H_

#include <string>
using namespace std;

#include "logic.h"

#include "data.h"

using namespace eda;

class InstructionFactoryARM : public InstructionFactory {
public:
  Address* Process(Address* start);
  void InitRegisters(Memory* m);
};

namespace ARM {

const string conditions[16] = { "EQ", "NE", "HS", "LO", "MI", "PL",
  "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "", "" };
const string registers[18] = { "R0", "R1", "R2", "R3", "R4", "R5", "R6",
  "R7", "R8", "R9", "R10", "R11", "R12", "SP", "LR", "PC", "CPSR", "SPSR" };
const string opcodes[16] = { "AND", "XOR", "SUB", "RSB", "ADD", "ADC",
  "SBC", "RSC", "TST", "TEQ", "CMP", "CMN", "ORR", "MOV", "BIC", "MVN" };
const string shifts[4] = { "LSL", "LSR", "ASR", "ROR" };

// All based off CPSR, CPSR is NZCV...
const string Z = "(([`CPSR`] >> 30) & 1)";
const string C = "(([`CPSR`] >> 29) & 1)";
const string N = "(([`CPSR`] >> 31) & 1)";
const string V = "(([`CPSR`] >> 28) & 1)";

const string conditions_absolute[16] = {
  Z,     // EQ, Z set
  "~"+Z, // NE, Z clear
  C,     // HS, C set
  "~"+C, // LO, C clear
  N,     // MI, N set
  "~"+N, // PL, N clear
  V,     // VS, V set
  "~"+V, // VC, V clear
  C+"&~"+Z,  // HI, C set and Z clear
  Z+"|~"+C,  // LS, C clear or Z set
  //GE, N set and V set or N clear and V clear
  "~("+N+"^"+V+")",
  //LT, N set and V clear or N clear and V set
  N+"^"+V,
  //GT, Z clear and either N set and V set or N clear and V clear
  "~"+Z+"&~("+N+"^"+V+")",
  "1",
  "1"
};

const string opcodes_absolute[16] = {
// C is update carry
// NS is no store
// NF is no first(Rn)
//AND XOR SUB RSB    ADD ADC SBC  RSC   TST TEQ CMP CMN ORR MOV BIC  MVN
//                        C   C    C    NS  NS  NS  NS      NF       NF
  "&","^","-","*-1+","+","+","-","*-1+","&","^","-","+","|","", "&~","~"
};

#define F_C 1
#define F_NS 2
#define F_NF 4

const int opcodes_flags[16] = {
//AND XOR SUB RSB    ADD ADC SBC  RSC   TST  TEQ  CMP  CMN  ORR MOV  BIC  MVN
//                        C   C    C    NS   NS   NS   NS       NF        NF
  0,  0,  0,  0,     0,  F_C,F_C,F_C,   F_NS,F_NS,F_NS,F_NS,0,  F_NF,0,   F_NF
};

const string shifts_absolute[4] = { "<<", ">>", ">>>", ">/>" };

const string coprocessors[16] = { "P0", "P1", "P2", "P3", "P4", "P5", "P6",
                "P7", "P8", "P9", "P10", "P11", "P12", "P13", "P14", "P15" };

const string cp_registers[16] = { "C0", "C1", "C2", "C3", "C4", "C5", "C6",
                "C7", "C8", "C9", "C10", "C11", "C12", "C13", "C14", "C15" };

}

#endif
