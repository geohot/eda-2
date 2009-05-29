// logic.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3,

#ifndef EDA_LOGIC_H_
#define EDA_LOGIC_H_

#include "data_memory.h"

namespace eda {

class MemoryManager {

};

// This is an abstract class to be extended by the different arches
// If I thought this through right, no other class needs to be
// This also kills the ARM/Thumb problem
// EDA plugins are extensions to this
class InstructionFactory {
public:
  // Sets up the addresses of the registers
  virtual void InitRegisters(Memory *m) = 0;

  // Parses an instruction
  // instruction should be null when passed in
  // Returns the address after the end of this instruction
  virtual Address* Process(Address* start) = 0;
};

// This creates changelists
// Currently, theres two ways
class ChangelistFactory {
// out should be null when passed in to all
public:
  ChangelistFactory();
  // Used for loading from files or straight web data
  Changelist* CreateFromInput(Address* owner, const string& data, Address* start);
  // Used by the Core
  Changelist* CreateFromStatelessChangelist(Address* owner, StatelessChangelist& in,
                                     Memory* state);
private:
  // This is incremented every time a changelist is created
  // It starts out at zero
  int current_changelist_number_;
};

class Core {

};

}

#endif

