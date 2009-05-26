// FactoryOwner.h -- May 21, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// Here's where all the command parsing stuff lives. It also owns the factories and Memory

#include <vector>
#include <string>

#include "InstructionFactoryARM.h"
#include "data_atomic.h"
#include "data_memory.h"

namespace eda {

class FactoryOwner {
public:
  FactoryOwner();
  bool HandleGetRequest(const std::vector<std::string>& argv, std::string* out);
private:
  ChangelistFactory changelist_factory_;
  // Something has got to be done about the InstructionFactories
  // REGISTER_ARCH macro or something
  InstructionFactoryARM instruction_factory_arm_;
  Memory memory_;   // I also own the memory
};

}
