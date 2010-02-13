// JSON.cc -- July 22, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "JSON.h"
#include "data.h"

#include "util.h"

using namespace eda;

JSON::JSON() : first_(true), internal_("{") {
}

// root
void JSON::add(const string& name, const string& object) {
  preface(name);
  internal_ += '"';
  internal_ += object;
  internal_ += "\"";
}
void JSON::add(const string& name, int object) {
  preface(name);
  ostringstream temp;
  temp << object;
  internal_ += temp.str();
}

// recursion
void JSON::add(const string& name, const JSON& object) {
  preface(name);
  internal_ += object.serialize();
}

// primitives
void JSON::add(const string& name, uint32_t object) {
  //add(name, immed(object));
  preface(name);
  ostringstream temp;
  temp << object;
  internal_ += temp.str();
}

// output
string JSON::serialize() const {
  return internal_ + "}";
}

void JSON::preface(const string& name) {
  if(!first_) internal_ += ", ";
  first_ = false;
  if(name != "") {
    internal_ += '"';
    internal_ += name;
    internal_ += "\": ";
  }
}