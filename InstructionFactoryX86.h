// InstructionFactoryX86.h -- May 16, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#ifndef EDA_INSTRUCTIONFACTORYX86_H_
#define EDA_INSTRUCTIONFACTORYX86_H_

#include <string>
using namespace std;

#include "logic.h"

#include "data.h"

namespace eda {

class InstructionFactoryX86 : public InstructionFactory {
public:
  Address* Process(Address* start);
  void InitRegisters(Memory* m);
};

}

#endif
