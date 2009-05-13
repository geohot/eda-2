// Address.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data.h"

namespace eda {

Address* Address::get8(int changelist_number, uint8_t* data) {
  if(changelist_number == 0)    // 0 is a link to the latest changelist
    (*data) = datamap_.rbegin()->second;
  else
    (*data) = (--datamap_.upper_bound(changelist_number))->second;
  return next_;
}

Address* Address::get16(int changelist_number, uint16_t* data) {
  uint8_t* m = (uint8_t*)data;
  Address* n;
  n = get8(changelist_number, m);
  return n->get8(changelist_number, m+1);
}

Address* Address::get32(int changelist_number, uint32_t* data) {
  uint8_t* m = (uint8_t*)data;
  Address* n;
  n = get8(changelist_number, m);
  n = n->get8(changelist_number, m+1);
  n = n->get8(changelist_number, m+2);
  return n->get8(changelist_number, m+3);
}

Address* Address::set8(int changelist_number, uint8_t data) {
  datamap_[changelist_number] = data;
}

Address* Address::set16(int changelist_number, uint16_t data) {
  Address* n;
  n = set8(changelist_number, data & 0xFF);
  return n->set8(changelist_number, (data>>8) & 0xFF);
}

Address* Address::set32(int changelist_number, uint32_t data) {
  Address* n;
  n = set8(changelist_number, data & 0xFF);
  n = n->set8(changelist_number, (data>>8) & 0xFF);
  n = n->set8(changelist_number, (data>>16) & 0xFF);
  return n->set8(changelist_number, (data>>24) & 0xFF);
}

void Address::set_next(Address* next) {
  next_ = next;
}

Address* Address::get_next() {
  return next_;
}

bool Address::set_name(const string& name) {
  name_ = name;
}

const string& Address::get_name() {
  return name_;
}

}
