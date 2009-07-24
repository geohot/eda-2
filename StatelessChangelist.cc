// StatelessChangelist.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// StatelessChangelist could conceptually be a base class of Changelist

#include "data_atomic.h"
#include "util.h"

#include <string>

#include "JSON.h"

using namespace std;

using namespace eda;

void StatelessChangelist::add_change(const string& lhs, const string& cond, int bytes, const string& value) {
  changes_.insert(make_pair(make_pair(lhs, cond), make_pair(bytes, value)));
}

bool StatelessChangelist::get_first_change(StatelessChangelistIterator* a) {
  if(changes_.size() > 0) {
    (*a) = changes_.begin();
    return true;
  }
  else
    return false;
}

bool StatelessChangelist::get_next_change(StatelessChangelistIterator* a) {
  ++(*a);
  return (*a) != changes_.end();
}

int StatelessChangelist::get_size() {
  return changes_.size();
}

void StatelessChangelist::SerializeToXML(ostringstream& out) {
  out << "<StatelessChangelist>";
  for (StatelessChangelistIterator it = changes_.begin(); it != changes_.end(); ++it) {
    out << "<change>";
    out << "<target>" << MakeWellFormedXML(it->first.first) << "</target>";
    out << "<condition>" << MakeWellFormedXML(it->first.second) << "</condition>";
    out << "<bytes>" << it->second.first << "</bytes>";
    out << "<value>" << MakeWellFormedXML(it->second.second) << "</value>";
    out << "</change>";
  }
  out << "</StatelessChangelist>";
}


void StatelessChangelist::SerializeToJSON(JSON* json) {
  vector<JSON> ret;
  for (StatelessChangelistIterator it = changes_.begin(); it != changes_.end(); ++it) {
    ret.push_back(JSON());
    ret.back().add("target", it->first.first);
    ret.back().add("condition", it->first.second);
    ret.back().add("bytes", it->second.first);
    ret.back().add("value", it->second.second);
    
  }
  json->add("StatelessChangelist", ret);
}