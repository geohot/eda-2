// StatelessChangelist.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// StatelessChangelist could conceptually be a base class of Changelist

#include "data_atomic.h"

#include <string>

using namespace std;

using namespace eda;

void StatelessChangelist::add_change(const string& lhs, int bits, const string& cond, const string& value) {
  changes_.insert(make_pair(make_pair(lhs, bits), make_pair(cond, value)));
}

StatelessChangelistIterator StatelessChangelist::get_first_change() {
  if(changes_.size() > 0)
    return changes_.begin();
  else
    return NULL;
}

bool StatelessChangelist::get_next_change(StatelessChangelistIterator* a) {
  ++(*a);
  return (*a) != changes_.end();
}

int StatelessChangelist::get_size() {
  return changes_.size();
}
