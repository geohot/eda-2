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
  bool HandlePostRequest(const std::vector<std::string>& argv, std::string* out);
  bool HandleEvalRequest(const std::vector<std::string>& argv, std::string* out);
  bool HandleReadRequest(const std::vector<std::string>& argv, std::string* out);
  bool HandleStepRequest(const std::vector<std::string>& argv, std::string* out);
  bool HandleRenameRequest(const std::vector<std::string>& argv, std::string* out);
  bool HandleDisassembleRequest(const std::vector<string>& argv, std::string* out);

  Memory memory_;   // I also own the memory
  ChangelistFactory changelist_factory_;

  InstructionFactory* instruction_factory_;
private:
  // Something has got to be done about the InstructionFactories
  // REGISTER_ARCH macro or something
};

}
