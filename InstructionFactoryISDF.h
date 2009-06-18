// InstructionFactoryISDF.h -- June 18, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This parses ISDF files, and behaves like a normal instruction factory

#ifndef EDA_INSTRUCTIONFACTORYISDF_H_
#define EDA_INSTRUCTIONFACTORYISDF_H_

#include <string>
using namespace std;

#include "logic.h"
#include "data.h"
#include "util.h"

namespace eda {

class InstructionFactoryISDF : public InstructionFactory {
public:
  InstructionFactoryISDF(string filename);
  Address* Process(Address* start);
private:
  string store_;
};

}

#endif
