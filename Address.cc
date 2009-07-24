// Address.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data.h"

#include "util.h"

#include "JSON.h"

namespace eda {

int Address::current_gai_ = 0;
  
void Address::Clear() {
  datamap_.clear();
  datamap_.insert(make_pair(0,0));
}

void Address::Clear32() {
  Clear();
  next_->Clear();
  next_->next_->Clear();
  next_->next_->next_->Clear();
}

void Address::SerializeToXML(ostringstream& out) {
  out << "<Address>" << std::hex;
  out << "<name>" << name_ << "</name>";
  out << "<size>" << size_ << "</size>";
  if(location_ != 0xFFFFFFFF) {
    out << "<location>" << immed(location_) << "</location>";
  }
  //LOG(DEBUG) << "  wrote location";
  out << "<values>";
  for(map<int, uint8_t>::iterator it = datamap_.begin(); it != datamap_.end(); ++it) {
    if(it->first != 0) {
      uint32_t data;
      get(it->first, &data);
      out << std::dec << "<C_" << it->first << ">" << std::hex << data << "</C_" << std::dec << it->first << ">";
    }
  }
  //LOG(DEBUG) << "  wrote values";
  out << "</values>";
  if(instruction_ != NULL)
    instruction_->SerializeToXML(out);
  //LOG(DEBUG) << "  wrote instruction";
  out << "</Address>";
}

void Address::SerializeToJSON(JSON* json) {
  JSON addr;
  addr.add("gai", gai_);
  addr.add("name", name_);
  addr.add("size", size_);
  addr.add("type", type_);
  if(location_ != 0xFFFFFFFF) {
    addr.add("location", location_);
  }
  // add values
  vector<JSON> values;
  for(map<int, uint8_t>::iterator it = datamap_.begin(); it != datamap_.end(); ++it) {
    if(it->first != 0) {
      uint32_t data = 0;
      get(it->first, &data);
      JSON value;
      value.add("change", it->first);
      value.add("data", data);
      values.push_back(value);
    }
  }
  addr.add("values", values);
  if(instruction_ != NULL)
    instruction_->SerializeToJSON(&addr);
  json->add("Address", addr);
}

// Currently, the host system endianness must match the systems
  
Address* Address::get(int changelist_number, uint32_t* data) {
  uint8_t* m = (uint8_t*)data;
  Address* n = this;
  for(int i = 0; i < size_; i++) {
    n = n->get8(changelist_number, m+i);
  }
  return n;
}
  
Address* Address::set(int changelist_number, uint32_t data) {
  uint8_t* m = (uint8_t*)&data;
  Address* n = this;
  for(int i = 0; i < size_; i++) {
    n = n->set8(changelist_number, m[i]);
  }
  return n;
}
  
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
  size_ = 4;
  type_ = "data";
  uint8_t* m = (uint8_t*)data;
  Address* n;
  n = get8(changelist_number, m);
  n = n->get8(changelist_number, m+1);
  n = n->get8(changelist_number, m+2);
  return n->get8(changelist_number, m+3);
}

Address* Address::set8(int changelist_number, uint8_t data) {
  datamap_[changelist_number] = data;
  return next_;
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
  return true;
}

const string& Address::get_name() {
  return name_;
}

void Address::set_location(uint32_t location) {
  location_ = location;
}

uint32_t Address::get_location() {
  return location_;
}

}
