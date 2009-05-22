// History.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// I am a database of sorts
// modifiers_ maps Addresses to changelists they own
// modified_ maps Addresses to changelists that changed them
// I could map right to changelists themselves, but the numbers way is cleaner

#include "util.h"
#include "data.h"

using namespace eda;

void History::AddCommited(Changelist* cl) {
  changelists_[cl->get_changelist_number()] = cl;

  modifiers_[cl->get_owner()].push_back(cl->get_changelist_number());

  ChangelistIterator it;
  cl->get_first_change(&it);
  do {
    modified_[it->first].push_back(cl->get_changelist_number());
  } while(cl->get_next_change(&it));
  INFO << "pushed " << cl->get_changelist_number() << endl;
}

Changelist* History::get_changelist(int changelist_number) {
  map<int, Changelist*>::iterator it = changelists_.find(changelist_number);
  if (it != changelists_.end())
    return it->second;
  else
    return NULL;
}

// These need to turn into XML

vector<int>* History::get_modifiers(Address *a) {
  map<Address*, vector<int> >::iterator it = modifiers_.find(a);
  if (it != modifiers_.end())
    return &it->second;
  else
    return NULL;
}

vector<int>* History::get_modified(Address *a) {
  map<Address*, vector<int> >::iterator it = modified_.find(a);
  if (it != modified_.end())
    return &it->second;
  else
    return NULL;
}
