// History.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// I am a database of sorts
// modifiers_ maps Addresses to changelists they own
// modified_ maps Addresses to changelists that changed them
// I could map right to changelists themselves, but the numbers way is cleaner

#include "data.h"
#include "util.h"

using namespace eda;

void History::AddCommited(Changelist* cl) {
  changelists_[cl->get_changelist_number()] = cl;

  owned_[cl->get_owner()].push_back(cl->get_changelist_number());

  ChangelistIterator it;
  cl->get_first_change(&it);
  do {
    xrefs_[it->first].push_back(cl->get_changelist_number());
  } while(cl->get_next_change(&it));
  LOG(INFO) << "pushed " << cl->get_changelist_number();
}

Changelist* History::get_changelist(int changelist_number) {
  map<int, Changelist*>::iterator it = changelists_.find(changelist_number);
  if (it != changelists_.end())
    return it->second;
  else
    return NULL;
}

// These need to turn into XML

vector<int>* History::get_owned(Address *a) {
  map<Address*, vector<int> >::iterator it = owned_.find(a);
  if (it != owned_.end())
    return &it->second;
  else
    return NULL;
}

vector<int>* History::get_xrefs(Address *a) {
  map<Address*, vector<int> >::iterator it = xrefs_.find(a);
  if (it != xrefs_.end())
    return &it->second;
  else
    return NULL;
}
