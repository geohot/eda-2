// data_memory.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This is the main EDA database class

#include "data.h"

namespace eda {

// The main EDA database store
class Memory {
  // The destruction of a Memory object is too much for EDA
public:
  // Create an empty chunk of memory
  void AllocateChunk(const string& name_first, int length);
  void AllocateChunk(uint32 address_32, int length);

  // Save and Load from file
  // These are going to be very hard to write
  // They should call Serialize and Deserialize functions in every subclass
  // These will be written closer to the end
  void LoadFromFile(const string& filename);
  void SaveToFile(const string& filename);

  // Resolve a memory access
  // Does a lookup in the space_ map
  Address* MMU(uint32 address_32);

  // Commit a changelist to Memory
  // Also add it to the history
  void Commit(Changelist* change);

private:
// This is the actual backend store for memory segments
// A pointer to these elements exists in either space_ or named_
// This could be used for garbage collection
  vector<vector<Address> > segments_;

// An address can exist in either of these or both
// Registers do not exist in space_, just named_
  map<uint32, Address* > space_;
  vector<string, Address* > named_;

// The entire history of this memory
  History history_;
};

}
