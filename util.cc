// util.h -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include <string>
#include <sstream>
#include <iomanip>

#include "util.h"
#include "data.h"

using namespace std;
using namespace eda;

uint32_t eda::stoi(const string& num) {
  istringstream in;
  in.str(num);
  uint32_t ret;
  if(num.substr(0,2)=="0x")
    in >> std::hex >> ret;
  else if(num.find_first_of("abcdefABCDEF") != string::npos)
    in >> std::hex >> ret;
  else
    in >> std::dec >> ret;
  return ret;
}

std::string eda::MakeWellFormedXML(const std::string& in) {
  string ret;
  for(int i = 0; i<in.length(); i++) {
    switch(in[i]) {
      case '>': ret += "&gt;"; break;
      case '<': ret += "&lt;"; break;
      case '&': ret += "&amp;"; break;
      default: ret+=in[i]; break;
    }
  }
  return ret;
}

bool eda::file_to_string(const std::string& filename, std::string* out) {
  FILE* f = fopen(filename.c_str(), "rb");
  if (f==0) return false;
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* dat = (char*)malloc(size);
  int read = fread(dat, 1, size, f);
  out->assign(dat, size);
  delete [] dat;
  return read == size;
}

int eda::find_matching(const std::string& s, int start, char open, char close) {
  //cout << s << endl;
  int count = 0;
  for (int i = start; i < s.length(); i++) {
    if (s[i] == close) {
      if (count == 1) {
        //cout << "returning " << i << endl;
        return i;
      }
      else count--;
    } else if (s[i] == open) {
      count++;
    }
  }
  LOG(WARNING) << "error in brackets";
  return -1;
}

void eda::SerializeToXML(ostringstream& out, const vector<int>* v, string name, string item) {
  out << "<" << name << ">";
  if (v != NULL) {
    for (int i = 0; i < v->size(); i++) {
      out << "<" << item << ">";
      out << (*v)[i];
      out << "</" << item << ">";
    }
  }
  out << "</" << name << ">";
}

void eda::StringSplit(const char* a, const string& in, vector<string>* argv) {
  size_t start=in.find_first_not_of(a,0);
  size_t end=in.find_first_of(a, start);
  while(end!=std::string::npos || start!=std::string::npos)
  {
    argv->push_back(in.substr(start,end-start));
    start=in.find_first_not_of(a,end);
    end=in.find_first_of(a, start);
  }
}

// Returns things like 9 and 0x4F
string eda::immed(uint32_t data) {
  ostringstream o;
  if(data < 10) o << data;
  else o << hex << "0x" << data;
  return o.str();
}
// Signed
string eda::immed_signed(int32_t data) {
  ostringstream o;
  if(data<0) {
    o << "-";
    data = data * -1;
  }
  o << immed(data);
  return o.str();
}

eda::Logging::Logging(int level, const char* file, const char* pretty_function, const char* function, int line_number) {
  switch(level) {
    case 0:
      stream_ << file << "(" << line_number << "): ";
      break;
    case 1:
    case 2:
      stream_ << file << "(" << line_number << ")--" << pretty_function << ": ";
      break;
  }
}
