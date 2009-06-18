// InstructionFactoryARM.cc -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This should become generic and read core parser files

#include "InstructionFactoryARM.h"

#include "data_memory.h"
#include "data.h"

#include "util.h"

using namespace eda;

using namespace ARM;

void InstructionFactoryARM::InitRegisters(Memory* m) {
  for (int i = 0; i < 18; i++)
    registers_.push_back(make_pair(registers[i], m->AllocateSegment(registers[i], 4)));
  program_counter_ = m->get_address_by_name("PC");
  link_register_ = m->get_address_by_name("LR");
  stack_pointer_ = m->get_address_by_name("SP");
}

// Could go in InstructionFactory
void InstructionFactoryARM::StateToXML(std::ostringstream& out) {
  out << std::hex;
  out << "<Core>";
  out << "<ProgramCounter>" << GetProgramCounter() << "</ProgramCounter>";
  //out << "<LinkRegister>" << GetLinkRegister() << "</LinkRegister>";
  out << "<StackPointer>" << GetStackPointer() << "</StackPointer>";
  out << "<registers>";

  for(vector<pair<string, Address*> >::iterator it = registers_.begin(); it!=registers_.end(); ++it) {
    uint32_t data;
    it->second->get32(0, &data);
    out << "<" << it->first << ">" << data << "</" << it->first << ">";
  }

  out << "</registers>";
  out << "</Core>";
}

// I think this is my fifth one of these...haha
// This is going to be the simplest
// Extract everything first

Address* InstructionFactoryARM::Process(Address* start) {
  StatelessChangelist* change = new StatelessChangelist();

  uint32_t opcode;
  Address* end = start->get32(0, &opcode);  // 0 is a shortcut

  // Extract register data
  string Rn = registers[ (opcode >> 16) & 0xF ];
  string Rd = registers[ (opcode >> 12) & 0xF ];

  bool RdisPC = (((opcode >> 12) & 0xF) == 15);
  // Set if PC is changed by other crap
  bool changedPC = false;

  string Rs = registers[ (opcode >> 8) & 0xF ];
  string Rm = registers[ (opcode >> 0) & 0xF ];

  // Extract immediate data
  int immed24_numeric = ((opcode & 0x7FFFFF) << 2) - ((opcode & 0x800000)?0x2000000:0);
  string immed24 = immed_signed( immed24_numeric );

  int immed12_numeric = opcode & 0xFFF;
  string immed12 = immed( immed12_numeric );

  string immed8 = immed( ror( (opcode & 0xFF), ((opcode >> 8) & 0xF) * 2) );

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
  bool updateflags = ((opcode >> 20) & 1);  // || (opcodes_flags[opint] & F_NS);
  bool link = ((opcode >> 24) & 1);
  bool load = ((opcode >> 20) & 1);
  bool byte = ((opcode >> 22) & 1);
  bool reg = ((opcode >> 4) & 1);
  bool sign = ((opcode >> 23) & 1);

  bool writeback = ((opcode >> 21) & 1);
  bool preincrement = ((opcode >> 24) & 1);
  bool increment = ((opcode >> 23) & 1);

  // Parsed Instructionness
  string formatstring = "";
  vector<string> args;

  int cmdint = (opcode >> 25) & 7;

  string changesource = "";

  string operand = "";

  // Debugging
  /*cout << "Op:     " << hex << opcode << endl;
  cout << "Type:   " << cmdint << endl;
  cout << "Rn:     " << Rn << endl;
  cout << "Rd:     " << Rd << endl;
  cout << "Rs:     " << Rs << endl;
  cout << "Rm:     " << Rm << endl;

  cout << "Shift:  " << shiftXXX << "  " << shift << endl;
  cout << "Cond:   " << condXX << "  " << cond << endl;
  cout << "Opcode: " << opXXX << "  " << op << endl;

  cout << "immed24:" << immed24 << endl;
  cout << "immed12:" << immed12 << endl;
  cout << "immed8: " << immed8 << endl;*/

  // c++ is obnoxious
  int reglist = opcode & 0xFFFF;
  int rnum = 0;
  int offset = 0;
  uint32_t data;

  switch (cmdint) {
    case 0:   // DPIS + DPRS
    case 1:   // DPI
      formatstring += "OFC ";
      args.push_back(opXXX);
      args.push_back(((opcode >> 20) & 1)?"S":"");
      args.push_back(condXX);

      if (!(opcodes_flags[opint] & F_NS)) {
        formatstring += "R, ";
        args.push_back(Rd);
      }

      if (!(opcodes_flags[opint] & F_NF)) { // First register
        formatstring += "R, ";
        args.push_back(Rn);
        changesource += "[`"+Rn+"`]";
      }

      if (cmdint == 0) {
        formatstring += "R o ";
        args.push_back(Rm);
        args.push_back(shiftXXX);
        changesource += op;
        operand += "[`"+Rm+"`]" + shift;

        if (reg) { // Register shift
          formatstring += "R";
          args.push_back(Rs);
          operand += "[`"+Rs+"`]";
        } else {  // Immed shift
          formatstring += "#I";
          args.push_back(immedshift);
          operand += immedshift;
        }
      } else {
        formatstring += "#I";
        args.push_back(immed8);
        changesource += op;
        operand += immed8;
      }
      operand += ")";
      if (!(opcodes_flags[opint] & F_NS)) { // If not No set
        change->add_change("`"+Rd+"`", cond, 4, changesource+operand);
      }

      // All based off CPSR, CPSR is NZCV...
      //cout << "updateflags: " << updateflags << endl;
      if (updateflags) {
        string flags = "([`CPSR`] & 0x0FFFFFFF)";
        flags += " | (((" + changesource + operand + ")) & 0x80000000)";   // N
        flags += " | ((((" + changesource + operand + "))==0)<<30)";      // Z
        if (opint == 2 || opint == 6 | opint == 10) {  // SUB or SBC or CMP
          flags += " | (([`"+Rn+"`] < (" + operand +") << 29)";
        }
        else if (opint == 3 || opint == 7) {  // RSB or RSC
          flags += " | (([`"+Rn+"`] > (" + operand +") << 29)";
        }
        else if (opint == 4 || opint == 5 || opint == 11) {  // ADD, ADC, or CMN
          flags += " | ((([`"+Rn+"`] >> 2) & (((" + operand + ") >> 2)) & 0x2000000)";
        }

        // C is damn confusing
        // so is V
        change->add_change("`CPSR`", cond, 4, flags);
      }

      break;
    case 2:   //LSIO
    case 3:   //LSRO
      formatstring += "FOFC R, ";
      args.push_back(load?"LD":"ST");
      args.push_back("R");
      args.push_back(byte?"B":"");
      args.push_back(condXX);
      args.push_back(Rd);
      if(preincrement)
        formatstring += "[R, ";
      else
        formatstring += "[R], ";

      args.push_back(Rn);

      changesource = "[`"+Rn+"`]";
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
      if(preincrement)
        formatstring += "]";

      // Second PC Hack
      // immed12 may not be the only choice
      if(Rn == "PC" && start->memory_->get_address_by_location((start->get_location() + immed12_numeric + 8)) != NULL) {  // Shouldn't be a string compare
        formatstring = "FOFC R, =I";
        //LOG(INFO) << "location is " << std::hex << (start->get_location() + immed12_numeric + 8);
        start->memory_->get_address_by_location((start->get_location() + immed12_numeric + 8))->get32(0, &data);
        args[5] = immed(data);
      }

      if(load) {
        if(byte) {
          change->add_change("`"+Rd+"`", cond, 1, "["+changesource+"]");
        } else {
          changedPC = RdisPC;
          if(changedPC)
            change->add_change("`"+Rd+"`", cond, 4, "["+changesource+"]+8");
          else
            change->add_change("`"+Rd+"`", cond, 4, "["+changesource+"]");
        }
      } else {  //store
        if(preincrement) {
          change->add_change(changesource, cond, byte?1:4, "[`"+Rd+"`]");
        } else {
          change->add_change("[`"+Rn+"`]", cond, byte?1:4, "[`"+Rd+"`]");
          change->add_change("`"+Rn+"`", cond, byte?1:4, changesource);
        }
      }
      break;
    case 4: //LSM
      // do later
      formatstring += "FOFFC RF, {";
      args.push_back(load?"LD":"ST");
      args.push_back("M");
      args.push_back(increment?"I":"D");
      args.push_back(preincrement?"B":"A");
      args.push_back(condXX);
      args.push_back(Rn);
      args.push_back(writeback?"!":"");

      if (preincrement) {
        if (increment) offset += 4;
        else offset -= 4;
      }
      while(reglist != 0) {
        if(reglist&1) {
          formatstring += "R";
          if((reglist>>1) != 0) formatstring += ", ";
          args.push_back(registers[rnum]);
          if(load)
            change->add_change("`"+registers[rnum]+"`", cond,4, "[[`"+Rn+"`]+"+immed(offset)+"]");
          else
            change->add_change("[`"+Rn+"`]+"+immed(offset), cond, 4, "[`"+registers[rnum]+"`]");
          if (increment) offset += 4;
          else offset -= 4;
        }
        reglist >>= 1;
        rnum++;
      }
      if (writeback) {
        if (preincrement) {
          if (increment) offset -= 4;
          else offset += 4;
        }
        change->add_change("`"+Rn+"`", cond,4, "[`"+Rn+"`]+"+immed(offset));
      }
      formatstring += "}";
      break;
    case 5: // B, BL
      formatstring += "OFC P";
      args.push_back("B");
      args.push_back(link?"L":"");
      args.push_back(condXX);
      //args.push_back(immed24);
      // One PC Hack
      args.push_back(immed(start->get_location() + immed24_numeric + 8));

      change->add_change("`PC`",  cond, 4, "[`PC`]+8+"+immed24);
      changedPC = true;
      if(link) {
        change->add_change("`LR`", cond, 4, "[`PC`]+4");
        change->add_change("`PC`", "0", 4, "[`PC`]+4");   // Never actually happens, but you know, I'm hacking
      }
      break;
    case 6: // CoProcessor load/store & double register transfers
      //TODO : LDC(2)/STC(2)
      break;
    case 7: // CoProcessor data processing & register transfers , Software Interrupt
      if((opcode >> 24) == 0xff) // Undefined Instruction
        break;
      else if(((opcode >> 24) & 0xf) == 0xf) { // Software Interrupt
        formatstring = "OC I";
        args.push_back("SWI");
        args.push_back(condXX);
        args.push_back(immed(opcode & 0xffffff));
        break;
      }
      // CoProcessor data processing & register transfers
      bool cp_register_transfers = (opcode >> 4) & 1;
      short crm = opcode & 0xf;
      short cp_num = (opcode >> 8) & 0xf;
      short from_cp = (opcode >> 20) & 1; // MCR or MRC
      short cp_opcode_1 = cp_register_transfers ?
          ((opcode >> 21) & 7) : ((opcode >> 20) & 0xf);
      short cp_opcode_2 = (opcode >> 5) & 7;
      bool cp_v2 = ((opcode >> 28) == 0xf); // Only ARMv5 and above

      formatstring = cp_v2 ? "O" : "OC";
      formatstring += " o, I, R, o, o";

      if(!cp_register_transfers || (cp_opcode_2 != 0))
        formatstring += ", I";

      string cp_instruction =
          (cp_register_transfers ? (from_cp ? "MRC" : "MCR") : "CDP");
      if(cp_v2) cp_instruction += "2";
      args.push_back(cp_instruction);

      if(!cp_v2) args.push_back(condXX);
      args.push_back(coprocessors[cp_num]);
      args.push_back(immed(cp_opcode_1));
      args.push_back(Rd);
      args.push_back(cp_registers[(opcode >> 16) & 0xF]);
      args.push_back(cp_registers[crm]);
      if(!cp_register_transfers || (cp_opcode_2 != 0))
        args.push_back(immed(cp_opcode_2));
      break;
  }

  //cout << "changedPC: " << changedPC << endl;
  if(!changedPC)
    change->add_change("`PC`", "1", 4, "[`PC`]+4");
  else if(changedPC && cond != "1")
    change->add_change("`PC`", "~("+cond+")&1", 4, "[`PC`]+4");


  ParsedInstruction* parsed = new ParsedInstruction(formatstring, args);

  start->set_instruction(new Instruction(parsed, change, start, 4));
  return end;
}
