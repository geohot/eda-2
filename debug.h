// debug.h -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// Various functions to assist debugging

#include "util.h"
#include "data.h"
#include "logic.h"

namespace eda {

//DebugPrint is overloaded to handle various EDA types
void DebugPrint(Changelist* a);
void DebugPrint(ParsedInstruction* a);
void DebugPrint(StatelessChangelist* a);
void DebugPrint(vector<int>* v);
}
