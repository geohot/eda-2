// JSON.h -- July 22, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#ifndef EDA_JSON_H_
#define EDA_JSON_H_

#include <vector>
#include <string>
#include <sstream>

using namespace std;

namespace eda {

class JSON;

class JSON {
public:
  JSON();
  // root
  void add(const string& name, const string& object);
  void add(const string& name, int object);
  // recursion
  void add(const string& name, const JSON& object);
  // primitives
  void add(const string& name, uint32_t object);
  // array and pointers(err maybe)
  //template <typename T>
  //void add(const string& name, const vector<T>& objects);
  
  // output
  string serialize() const;
  
private:
  void preface(const string& name);
  string internal_;
  bool first_;
public:
  template <typename T>
  void add(const string& name, const vector<T>& objects) {
    preface(name);
    internal_ += "[";
    first_ = true;
    for(typename vector<T>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
      add("", *it);
    }
    internal_ += "]";
    first_ = false;
  }
};

}

#endif
