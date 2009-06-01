// Instruction.cc -- May 29, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data.h"

using namespace eda;
using namespace std;

void Instruction::SerializeToXML(ostringstream& out) {
  out << "<Instruction>";
  parsed_->SerializeToXML(out);
  change_->SerializeToXML(out);
/*  out << "<metadata>";
  for(std::vector<Instruction>)
  out << "</metadata>";*/
  out << "</Instruction>";
}
