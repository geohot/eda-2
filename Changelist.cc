// Changelist.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

#include "data_atomic.h"

namespace eda {

void Changelist::add_change(Address* target, uint8_t data) {
  changes_.insert(make_pair(target, data));
}

void Changelist::add_read(Address* source) {
  read_.push_back(source);
}

bool Changelist::get_first_change(ChangelistIterator* a) {
  if(changes_.size() > 0) {
    (*a) = changes_.begin();
    return true;
  } else
    return false;
}

bool Changelist::get_next_change(ChangelistIterator* a) {
  ++(*a);
  return (*a) != changes_.end();
}

Address* Changelist::get_owner() {
  return owner_;
}

int Changelist::get_changelist_number() {
  return changelist_number_;
}

int Changelist::get_size() {
  return changes_.size();
}

}
