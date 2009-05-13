// debug.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// Various functions to assist debugging

#include "debug.h"

#include <iostream>
#include <iomanip>

using namespace std;

void eda::DebugPrint(Changelist* a) {
  if (a == NULL) {
    cout << "This is a null pointer" << endl;
    return;
  }
  cout << "Changelist number: " << a->get_changelist_number() << endl;
  cout << "Owned by: " << a->get_owner()->get_name() << endl;
  if (a->get_size() > 0) {
    ChangelistIterator it = a->get_first_change();
    do {
      cout << setw(4) << hex << it->first->get_name() << " = " << (uint32_t)it->second << endl;
    } while(a->get_next_change(&it));
  }
  else
    cout << "This is a blank Changelist" << endl;
}

void eda::DebugPrint(StatelessChangelist* a) {
  if (a == NULL) {
    cout << "This is a null pointer" << endl;
    return;
  }
  if (a->get_size() > 0) {
    StatelessChangelistIterator it = a->get_first_change();
    do {
      cout << setw(4) << it->first.first << "(" << it->first.second << ") (" << it->second.first << ")= " << it->second.second << endl;
    } while(a->get_next_change(&it));
  }
  else
    cout << "This is a blank Changelist" << endl;
}

void eda::DebugPrint(vector<int>* v) {
  for (vector<int>::iterator it = v->begin(); it != v->end(); ++it) {
    cout << (*it) << " ";
  }
  cout << endl;
}

void eda::DebugPrint(ParsedInstruction* i) {
  for (vector<string>::iterator it = i->args_.begin(); it != i->args_.end(); ++it) {
    cout << *it;
  }
  cout << endl;
}
