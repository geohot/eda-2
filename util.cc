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
  in >> ret;
  return ret;
}
