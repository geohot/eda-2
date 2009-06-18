// InstructionFactoryISDF.h -- June 18, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This parses ISDF files, and behaves like a normal instruction factory

#ifndef EDA_INSTRUCTIONFACTORYISDF_H_
#define EDA_INSTRUCTIONFACTORYISDF_H_

#include <string>
using namespace std;

#include "logic.h"
#include "data.h"
#include "util.h"

namespace eda {

class InstructionFactoryISDF : public InstructionFactory {
public:
  InstructionFactoryISDF(string filename);
  Address* Process(Address* start);
private:
  //All the InstructionComprehensions...yay!
  vector<InstructionComprehension> instructioncomprehensions_;

  // This is the global scope after a first read
  // Process makes a copy of this
  map<string, string> global_scope_;

  // This is the whole file that was read
  string store_;
};

class InstructionComprehension {
public:
  InstructionComprehension(const vector<string>& lines);
  // Given an opcode, check if this is a match
  bool IsMatch(uint32_t data);

  // Execute this instruction comprehension given the global scope which it can modify
  // It also needs
  void Execute(map<string, string>* global_scope, StatelessChangelist* change, ParsedInstruction* parsed);
private:
  // The Instruction data and the match mask
  uint32_t data_;
  uint32_t mask_;

  // Needs a fast internal representation
  map<string, string> global_scope_additions_;

  // We could use a StatelessChangelist, but most methods wouldn't be relevant
  map<string, string> changes_;

  // If it has a parsed
  // One parsed per comprehension
  string parsed_format_;
  vector<string> parsed_atoms_;
  int parsed_format_index_; // Index into parsedformat to splice, -1 means clear parsed
  int parsed_atoms_index_;  // Index into parsedatoms to splice
};

// Called by InstructionComprehension.Execute
void EvalulateStringInScope(const map<string, string>& global_scope, string* evalme) {
  // Find {...} shit and replace it
}

}

#endif
