// ParsedInstruction.cc -- May 16, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data_atomic.h"

using namespace std;
using namespace eda;

// O -- opcode
// o -- subopcode
// F -- flags
// C -- condition
// R -- register
// I -- immed
// P -- PC offset immed
// p -- dereferenced PC offset immed

string ParsedInstruction::GetConsoleString() {
  string out;
  int vpos = 0;
  for (int i = 0; i < format_.length(); i++) {
    switch(format_[i]) {
      case 'O':
      case 'o':
      case 'F':
      case 'C':
      case 'R':
      case 'I':
      case 'P':
      case 'p':
        out += args_[vpos];
        vpos ++;
        break;
      default:
        out += format_[i];
    }
  }
  return out;
}

map<char, string> ParsedInstruction::web_lookup_;

void ParsedInstruction::SerializeToXML(ostringstream& out) {
  if(web_lookup_.empty()) {
    web_lookup_.insert('O', "opcode");
    web_lookup_.insert('o', "opcode");
    web_lookup_.insert('F', "flags");
    web_lookup_.insert('C', "flags");
    web_lookup_.insert('R', "register");
    web_lookup_.insert('I', "immed");
    web_lookup_.insert('P', "location");
    web_lookup_.insert('p', "immed");
  }
  out << "<parsedinstruction>";
  int vpos = 0;
  for (int i = 0; i < format_.length(); i++) {
    map<char, string>::iterator it = web_lookup_.find(format_[i]);
    if(it != web_lookup_.end()) {
      out << "<atom>";
      out << "<type>" << web_lookup_->second << "</type>";
      out << "<data>" << args_[vpos] << "</data>";
      vpos++;
      out << "</atom>";
    } else {
      out << "<formatting>"  << format_[i] << "</formatting>";
    }
  }

  out << "</parsedinstruction>";
}
