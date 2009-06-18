// InstructionFactoryISDF.cc -- June 18, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This parses ISDF files, and behaves like a normal instruction factory

#include "InstructionFactoryISDF.h"
#include <string>
#include <fstream>
#include <iostream>

#include "file.h"

using namespace std;
using namespace eda;

InstructionFactoryISDF::InstructionFactoryISDF(string filename) {
  File::ReadFileToString(kBaseDirectory+filename, &store_);
  int i = 0;
  while(i < store_.length()) {
    int pos = store_.find_first_of('\n', i);
    string line = store_.substr(i, pos);
    i += pos+1;

    cout << line << endl;
  }
}

Address* InstructionFactoryISDF::Process(Address* start) {
  return NULL;
}
