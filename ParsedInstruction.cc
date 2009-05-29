// ParsedInstruction.cc -- May 16, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data_atomic.h"

#include <map>
#include <string>

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
    web_lookup_.insert(make_pair('O', "opcode"));
    web_lookup_.insert(make_pair('o', "opcode"));
    web_lookup_.insert(make_pair('F', "flags"));
    web_lookup_.insert(make_pair('C', "flags"));
    web_lookup_.insert(make_pair('R', "register"));
    web_lookup_.insert(make_pair('I', "immed"));
    web_lookup_.insert(make_pair('P', "location"));
    web_lookup_.insert(make_pair('p', "immed"));
  }
  out << "<parsedinstruction>";
  int vpos = 0;
  bool lastformatting = false;
  for (int i = 0; i < format_.length(); i++) {
    map<char, string>::iterator it = web_lookup_.find(format_[i]);
    if(it != web_lookup_.end()) {
      if(lastformatting==true) {
        out << "</formatting>";
        lastformatting = false;
      }
      out << "<atom>";
      out << "<type>" << it->second << "</type>";
      out << "<data>" << args_[vpos] << "</data>";
      vpos++;
      out << "</atom>";
    } else {
      if(lastformatting==false) {
        out << "<formatting>";
        lastformatting = true;
      }
      out << format_[i];
    }
  }
  if(lastformatting==true) {
    out << "</formatting>";
    lastformatting = false;
  }

  out << "</parsedinstruction>";
}
