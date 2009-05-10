// logic.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3,

#ifndef EDA_LOGIC_H_
#define EDA_LOGIC_H_

#include "data.h"


class MemoryManager {

};

// This is an abstract class to be extended by the different arches
// If I thought this through right, no other class needs to be
// This also kills the ARM/Thumb problem
// EDA plugins are extensions to this
class InstructionFactory {
  InstructionFactory() = 0;

  // Parses an instruction
  // instruction should be null when passed in
  // Returns the address after the end of this instruction
  virtual static Address* process(Address* start, Instruction* instruction);
};

// This creates changelists
// Currently, theres two ways
class ChangelistFactory {
// out should be null when passed in to all
public:
  // Used for loading from files or straight web data
  bool CreateFromInput(const string& data, Address* start, Changelist* out);
  // Used by the Core
  bool CreateFromStatelessChangelist(StatelessChangelist* in,
                                     Memory* state, Changelist* out);
private:
  // This is incremented every time a changelist is created
  // It starts out at zero
  int current_changelist_number_;
};

class Core {

};

#endif

