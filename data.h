// data.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This contains all of EDA's data classes
// I can't include Logic or Presentation without screwing up

#ifndef EDA_DATA_H_
#define EDA_DATA_H_

#ifdef __linux
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include <map>
#include <set>

#include "data_atomic.h"

using namespace std;

// Memory
//   Address[]
//     Instruction
//     datamap_
//     name_

namespace eda {

class Memory;

// Instruction is no longer extended, InstructionFactory is
// Don't put the commit logic here, Instruction is a dumb storage class
class Instruction {
public:
  Instruction(ParsedInstruction* parser, StatelessChangelist* change, Address* start, int length) :
    parsed_(parser),
    change_(change),
    start_(start),
    length_(length) {}
  // This should have a destructor that destroys parsed_ and stateless_
  void SerializeToXML(std::ostringstream& out);
  void SerializeToJSON(JSON* json);

  void GetFunction(std::set<Address*>* addresses);
//private:


  // Metadata should be updated by the running too

  // All instructions that this instruction can accept control from
  // If > 1, this instruction begins a basic block
  // Conditional linked branches have to be dealt with
  std::vector<Address*> control_inputs_;

  // All instructions that this instruction can hand control to
  // Used for building a control flow graph
  // flag if this is a linked output, or a return output
  std::vector<Address*> control_outputs_;

  // Returns from functions
  std::vector<Address*> control_indirect_inputs_;

  // In here if this is a linked branch or a return
  std::vector<Address*> control_indirect_outputs_;

  ParsedInstruction* parsed_;
  StatelessChangelist* change_;
  Address* start_;
  int length_;
};

// An address is one entry in the Memory Database
// It isn't tied to a 32-bit number in the space, that's the MMUs job
// Registers are also addresses
// It can be accessed by 32-bit address, name, or changelist

// Users are a very special case of Address,
// They can submit changelists directly
// Many methods don't have much signifance to them
  
// gai is Global Address Identifier
// It is used to identify an address in the global EDA space
  
class Address {
public:
  Address(Memory* memory) {
    next_ = NULL;
    instruction_ = NULL;
    datamap_.insert(make_pair(0,0));
    name_ = "";
    memory_ = memory;
    location_ = 0xFFFFFFFF;
    size_ = 1;
    gai_ = current_gai_;
    current_gai_++;
  }
  // Address accessor functions
  // All return pointer to address after last one got
  // NULL if said address doesn't exist
  Address* get(int changelist_number, uint32_t* data);
  Address* get8(int changelist_number, uint8_t* data);
  Address* get16(int changelist_number, uint16_t* data);
  Address* get32(int changelist_number, uint32_t* data);
  // Address mutator functions are only to be called from commit
  Address* set(int changelist_number, uint32_t data);
  Address* set8(int changelist_number, uint8_t data);
  Address* set16(int changelist_number, uint16_t data);
  Address* set32(int changelist_number, uint32_t data);

  // Clear this memory location
  void Clear();
  void Clear32();

  // Operators to walk you through the address list
  //Address* operator++ (Address*);
  //Address* operator-- (Address*);

  void SerializeToXML(ostringstream& out);
  void SerializeToJSON(JSON* json);

  // Names can't start with numbers, enforce this
  bool set_name(const string& name);

  void set_instruction(Instruction* i) { instruction_ = i; }
  Instruction* get_instruction() { return instruction_; }

  void set_location(uint32_t location);
  uint32_t get_location();

  void set_next(Address* next);
  Address* get_next();
  const string& get_name();

  int get_size() { return size_; }
  void set_size(int size) { size_ = size; }

  int get_gai() { return gai_; }
  
  string type_;   // what type of address is this? instruction? data?
  
  Memory* memory_;  // The memory that created me
private:
  int gai_;
  static int current_gai_;
  
  uint32_t location_;

  // This is the DCB, DCW, DCD field
  int size_;

  //vector<string> flags;     //this is a possibility

  // Maps changelistNumbers to 8-bit datas
  map<int, uint8_t> datamap_;

  // Name of this address
  string name_;

  // If not null, the instruction at this address
  Instruction* instruction_;

  // These allow for traversing the address space
  // And let functions like get32 work
  //Address* prev_;
  Address* next_;
};

class History {
  // The destruction of a Memory object is too much for EDA
public:
  // Changelists never get deleted, so this gets a pointer
  // This also gets the current address when the instruction was commited
  void AddCommited(Changelist* cl);
  vector<int>* get_owned(Address *a);
  vector<int>* get_xrefs(Address *a);
  Changelist* get_changelist(int changelist_number);
private:
  // Save every changelist ever commited into the memory database
  map<int, Changelist*> changelists_;

  // Given an Address, I'd like to know what it has done
  // Map addresses to changelists that they commited
  // This can be built using get_owner
  map<Address*, vector<int> > owned_;

  // This can be generated by walking the
  // Map addresses to changelists that affected them
  map<Address*, vector<int> > xrefs_;
};


}

#endif
