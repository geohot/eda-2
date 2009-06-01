// data_memory.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This is the main EDA database class

#ifndef EDA_DATAMEMORY_H_
#define EDA_DATAMEMORY_H_

#include "data.h"

namespace eda {

// The main EDA database store
class Memory {
  // The destruction of a Memory object is too much for EDA
public:
  // Create an empty chunk of memory
  Address* AllocateSegment(const string& name_first, int length);
  Address* AllocateSegment(uint32_t address_32, int length);

  // Save and Load from file
  // These are going to be very hard to write
  // They should call Serialize and Deserialize functions in every subclass
  // These will be written closer to the end
  void LoadFromFile(const string& filename);
  void SaveToFile(const string& filename);

  // Resolve a memory access
  // Does a lookup in the space_ map
  Address* get_address_by_location(uint32_t address_32);
  Address* get_address_by_name(const string& name);

  // Rename an Address
  // Could move to Address, but then it would need to know what Memory owns it

  void Rename(Address* address, const std::string& name);

  // Commit a changelist to Memory
  // Also add it to the history
  void Commit(Changelist* change);

  // Resolve a complicated string
  // Output is filled
  uint32_t ResolveToNumber(int changelist_number, const string& stateless);
  Address* ResolveToAddress(int changelist_number, const string& stateless);

  // The entire history of this memory
  // Not sure what to do about the publicness of this
  History history_;

private:
// This is the actual backend store for memory segments
// A pointer to these elements exists in either space_ or named_
// This could be used for garbage collection
// If the addresses actually lived inside the vector,
// We couldn't resize segments
  vector<vector<Address*>* > segments_;

// Creates a segment but makes it inaccessible
// That's why it's private
  vector<Address*>* AllocateSegment(int length);

// An address can exist in either of these or both
// Registers do not exist in space_, just named_
  map<uint32_t, vector<Address*>* > space_;
  map<string, Address* > named_;
};

}

#endif
