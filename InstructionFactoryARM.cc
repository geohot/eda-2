// InstructionFactoryARM.cc -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This should become generic and read core parser files

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

  uint32 opcode;
  Address* end = start->get32(opcode);

  // Extract register data
  string Rn = registers[ (opcode >> 16) & 0xF ];
  string Rd = registers[ (opcode >> 12) & 0xF ];
  string Rs = registers[ (opcode >> 8) & 0xF ];
  string Rm = registers[ (opcode >> 0) & 0xF ];

  // Extract immediate data
  string immed24 =
    immed_signed( ((opcode & 0x7FFFFF) << 2) * ((opcode & 0x800000)?-1:1) );
  string immed12 = immed(opcode & 0x3FF);
  string immed8 = immed( rol(opcode & 0xFF, ((opcode >> 8) & 0xF) * 2) );

  // Extract shift information
  string immedshift = immed((opcode >> 7) & 0x1F);
  string shiftXXX = shifts[(opcode >> 5) & 3];
  string shift = shifts_absolute[(opcode >> 5) & 3];

  // Extract condition information
  string condXX = conditions[opcode >> 28];
  string cond = conditions_absolute[opcode >> 28];

  // Extract opcode information
  int opint = (opcode >> 21) & 0xF;
  string opXXX = opcodes[opint];
  string op = opcodes_absolute[opint];

  // Flags
  bool updateflags = ((opcode >> 20) & 1) || (opcodes_flags[opint] && F_NS);
  bool link = ((opcode >> 24) & 1);
  bool load = ((opcode >> 20) & 1);
  bool byte = ((opcode >> 22) & 1);
  bool reg = ((opcode >> 4) & 1);
  bool sign = ((opcode >> 23) & 1);

  // Parsed Instructionness
  string formatstring = "";
  vector<string> args;

  int cmdint = (opcode >> 25) & 3;

  switch (cmdint) {
    case 0:   // DPIS + DPRS
    case 1:   // DPI
      formatstring += "OFC R, ";
      args.push_back(opXXX);
      args.push_back(((opcode >> 20) & 1)?"S":"");
      args.push_back(condXX);
      args.push_back(Rd);

      string changesource = "";

      if (!(opint & F_NF)) { // First register
        formatstring += "R, ";
        args.push_back(Rn);
        changesource += "[`"+Rn+"`]";
      }

      if (cmdint == 0) {
        formatstring += "R o ";
        args.push_back(Rm);
        args.push_back(shiftXXX);
        changesource += op + "[`"+Rm+"`]" + shift;

        if (reg) { // Register shift
          formatstring += "R";
          args.push_back(Rs);
          changesource += "[`"+Rs+"`]";
        } else {  // Immed shift
          formatstring += "#I";
          args.push_back(immedshift);
          changesource += immedshift;
        }
      } else {
        formatstring += "#I";
        args.push_back(immed8);
        changesource += immed8;
      }
      change->add_change(make_pair("`"+Rd+"`", 32),
                         make_pair(cond, changesource));
      break;
    case 2:   //LSIO
    case 3:   //LSRO
      formatstring += "OFC R, [R, ";
      args.push_back(load?"LD":"ST");
      args.push_back(byte?"B":"");
      args.push_back(condXX);
      args.push_back(Rd);
      args.push_back(Rn);

      string changesource = "[";
      changesource += "`"+Rn+"`";
      if(sign) changesource += "+";
      else changesource += "-";
      if (cmdint == 2) {
        formatstring += "#I";
        if(sign) args.push_back(immed12);
        else args.push_back("-"+immed12);
        changesource += immed12;
      } else {
        formatstring += "R o #I";
        args.push_back(Rm);
        args.push_back(shiftXXX);
        if(sign) args.push_back(immedshift);
        else args.push_back("-"+immedshift);
        changesource += "[`"+Rm+"`]" + shift + immedshift;
      }
      changesource += "]";
      if(byte) {
        change->add_change(make_pair("[`"+Rd+"`]", 8),
                           make_pair(cond, changesource));
      } else {
        change->add_change(make_pair("[`"+Rd+"`]", 32),
                           make_pair(cond, changesource));
      }
      break;
    case 4: //LSM
      // do later
      break;
    case 5: // B, BL
      formatstring += "OFC P";
      args.push_back("B");
      args.push_back(link?"L":"");
      args.push_back(cond);
      args.push_back(immed24);

      change->add_change(make_pair("`PC`", 32),
                         make_pair(cond, "[`PC`]+"+immed24));
      if(link) {
        change->add_change(make_pair("`LR`", 32),
                           make_pair(cond, "[`PC`]+4"));
      }
      break;
  }


  ParsedInstruction* parsed = new ParsedInstruction(formatstring, args);

  instruction = new Instruction(parsed, change, start, 4);
  return end;
}
