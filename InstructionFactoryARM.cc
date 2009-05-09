// InstructionFactoryARM.cc -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "InstructionFactoryARM.h"

#include "data.h"

using namespace eda;

using namespace ARM;

// I think this is my fifth one of these...haha
// This is going to be the simplest
// Extract everything first
virtual static Address* InstructionFactoryARM::process(
  Address* start, Instruction* instruction) {
  StatelessChangelist* change = new StatelessChangelist();
  ParsedInstruction* parsed = new ParsedInstruction();

  uint32 opcode;
  Address* end = start->get32(opcode);

  // Extract register data
  string Rn = registers[ (opcode >> 16) & 0xF ];
  string Rd = registers[ (opcode >> 12) & 0xF ];
  string Rs = registers[ (opcode >> 8) & 0xF ];
  string Rm = registers[ (opcode >> 0) & 0xF ];

  // Extract immediate data
  string immed24 = immed(opcode & 0xFFFFFF);  // Probably need more here
  string immed12 = immed(opcode & 0x3FF);
  string immed8 = immed( rol(opcode & 0xFF, ((opcode >> 8) & 0xF) * 2) );

  // Extract condition information
  string condXX = conditions[opcode >> 28];
  string cond = conditions_absolute[opcode >> 28];

  // Extract opcode information
  string opXXX = opcodes[(opcode >> 21) & 0xF];
  string op = opcodes_absolute[(opcode >> 21) & 0xF];

  //...

  instruction = new Instruction(parsed, change, start, 4);
  return end;
}
