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
