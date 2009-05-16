// data_atomic.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This is the level under data
// Nothing should be included
// These are the lowest level data structures in EDA
// Currently, thats just the changelists

#ifndef EDA_DATAATOMIC_H_
#define EDA_DATAATOMIC_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace eda {

class Address;    // Can't actually call address

#define StatelessChangelistIterator map<pair<string, int>, pair<string, string> >::iterator

// This is a changelist that is "position independent" and "state independent"
// Should be initted as a string
// A StatelessData is now a string
class StatelessChangelist {
public:
  // This adds an assignment of the form
  // if(rhs.first)
  //   (lhs.second)lhs.first = rhs.second;
  // lhs.second is number of bits
  void add_change(const string& lhs, int bits, const string& cond, const string& value);

  // Returns first change in list
  bool get_first_change(StatelessChangelistIterator* a);
  bool get_next_change(StatelessChangelistIterator* a);
  int get_size();
private:
  map<pair<string, int>, pair<string, string> > changes_;
};

#define ChangelistIterator map<Address*, uint8_t>::iterator

// This is a set of instructions to modify explicit memory
class Changelist {
public:
  Changelist(int changelist_number, Address* owner) :
    changelist_number_(changelist_number),
    owner_(owner) {};
  // This adds an assignment of the form
  // lhs = rhs;
  void add_change(Address* target, uint8_t data);

  void add_read(Address* source);

  // owner_ accessor
  Address* get_owner();

  int get_changelist_number();
  int get_size();

  // Returns first change in list
  bool get_first_change(ChangelistIterator* a);
  bool get_next_change(ChangelistIterator* a);

  void DebugPrint();
private:
  // This is every change the changelist makes
  // Addresses are stored in their pointer form so no moving or renaming
  // affects them
  vector<Address*> read_; // Everything this changelist read
  map<Address*, uint8_t> changes_;
  int changelist_number_;
  Address* owner_;
};

// O -- opcode
// o -- subopcode
// F -- flags
// C -- condition
// R -- register
// I -- immed
// P -- PC offset immed
// p -- dereferenced PC offset immed
class ParsedInstruction {
public:
  // Is this valid c++?
  // Should be like printf for strings
  ParsedInstruction(const string& format, const vector<string>& args) : format_(format), args_(args) {}
  string GetConsoleString();
private:
// It'd be sweet if this was a string with formatting
  string format_;
  vector<string> args_;
};

}

#endif
