// InstructionFactoryISDF.h -- June 18, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This parses ISDF files, and behaves like a normal instruction factory
// Wonder how well I'll remember how to code this tomorrow...omg is it thursday?

#ifndef EDA_INSTRUCTIONFACTORYISDF_H_
#define EDA_INSTRUCTIONFACTORYISDF_H_

#include <string>
#include <map>
using namespace std;

#include "logic.h"
#include "data.h"
#include "util.h"

namespace eda {
class InstructionComprehension;

class InstructionFactoryISDF : public InstructionFactory {
public:
  InstructionFactoryISDF(const string& filename, Memory* m);
  bool LoadFromFile(const string& filename);
  Address* Process(Address* start);

  void StateToXML(std::ostringstream& out);

  // Called by InstructionComprehension.Execute
  // Find {...} shit and replace it
  string EvalulateStringInScope(const map<string, string>& global_scope, const map<char, uint32_t>& local_scope, const string& evalme);

  int current_line_;
  vector<string> line_vector_debug_;
private:
  //All the InstructionComprehensions...yay!
  vector<InstructionComprehension*> instructioncomprehensions_;

  map<string, pair<int, string> > default_changes_;

  // This is the global scope after a first read
  // Process makes a copy of this
  map<string, string> global_scope_;

  // This is the whole file that was read
  string store_;

  // Seriously remove this
  Memory* memory_;
};

class InstructionComprehension {
public:
  InstructionComprehension(const string& lines, InstructionFactoryISDF* ISDF);
  void AddLine(const string& linein);

  // Addresses are NULL after read
  void InitRegisters(Memory *m);

  // Given an opcode, check if this is a match
  // Execute this instruction comprehension given the global scope which it can modify
  // Returns true if stop flag was hit
  bool Execute(Address* opcode, map<string, string>* global_scope, StatelessChangelist* change, ParsedInstruction* parsed);
private:
  // The Instruction data and the match mask
  // Size is accounted for here, mask always blank on top for smaller instructions
  uint32_t data_;
  uint32_t mask_;

  // Number of bits
  int bitsize_;

  // char is variable name, int is the offset from the left for rightmost bit, uint32_t is the mask

  map<char, pair<int, uint32_t> > local_scope_additions_;

  // Needs a fast internal representation
  map<string, string> global_scope_additions_;

  // We could use a StatelessChangelist, but most methods wouldn't be relevant
  // bool is the Default flag
  map<string, pair<int, string> > changes_;

  // If it has a parsed
  // One parsed per comprehension
  string parsed_format_;
  vector<string> parsed_atoms_;
  int parsed_type_; // 0 = none, 1 = prepend, 2 = append, 3 = clear

  bool stop_flag_;

  // Line from ISDF file
  int line_;

  InstructionFactoryISDF* parent_;
};

}

#endif
