// ChangelistFactory.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// I make Changelists

#include "logic.h"
#include "util.h"

using namespace eda;

ChangelistFactory::ChangelistFactory() {
  current_changelist_number_ = 0;
}

Changelist* ChangelistFactory::CreateFromInput(Address* owner, const string& data, Address* start) {
  Changelist* out = new Changelist(current_changelist_number_++, owner);
  int i;
  for (i = 0; i < data.size() && start!=0; i++) {
    //cout << start << endl;
    out->add_change(start, data[i]);
    start = start->get_next();
  }
  if (i != data.size()) {
    LOG << "only added " << i << " changes" << endl;
  }
  return out;
}
