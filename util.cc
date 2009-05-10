// util.h -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include <string>
#include <sstream>

#include "util.h"

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
