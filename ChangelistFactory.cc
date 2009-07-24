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
  //Changelists start at 1 since every Address contains [0] = 0
  current_changelist_number_ = 1;
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
    LOG(INFO) << "only added " << i << " changes" << endl;
  }
  return out;
}

Changelist* ChangelistFactory::CreateFromStatelessChangelist(Address* owner, StatelessChangelist& in, Memory* state) {
  StatelessChangelistIterator it;
  if (!in.get_first_change(&it)) return NULL;
  Changelist* out = new Changelist(current_changelist_number_, owner);
  LOG(DEBUG) << "changelist is number " << current_changelist_number_;
  do {
    if (state->ResolveToNumber(current_changelist_number_, it->first.second) != 0) {   // Check the condition
      Address *target = state->ResolveToAddress(current_changelist_number_, it->first.first);
      uint32_t value = state->ResolveToNumber(current_changelist_number_, it->second.second);
      if (target != NULL) {
        /*LOG(INFO) << "setting address " << target->get_location() << " to data of size " << it->second.first;
        target->set_size(it->second.first);
        target->type_ = "data";*/
        for (int bc = 0; bc < it->second.first; bc++) {
          //LOG(DEBUG) << "adding change " << target << " = " << (value&0xFF);
          out->add_change(target, value & 0xFF);
          target = target->get_next();
          value >>= 8;
        }
      } else {
        LOG(INFO) << "Address not found " << it->first.first;
      }
    }
  } while (in.get_next_change(&it));
  current_changelist_number_++;
  return out;
}
