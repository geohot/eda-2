// util.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#ifndef EDA_UTIL_H_
#define EDA_UTIL_H_

#include <iostream>

namespace eda {

// Returns things like 9 and 0x4F
string immed(uint32 data);
// Signed
string immed_signed(int32 data);

uint32 stoi(const string& num);

bool file_to_string(const string& filename, string* out);

// LOG(INFO)
// LOG(ERROR)

#ifdef WIN32
#define THIS_FILE ((strrchr(__FILE__, 0x5C) ?: __FILE__ - 1) + 1)
#else
#define THIS_FILE ((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)
#endif

#define LOG std::cout << std::endl << std::hex << THIS_FILE << "--" << __PRETTY_FUNCTION__ << ": "

inline uint32 rol(uint32 data, int len);
inline uint32 ror(uint32 data, int len);

}

#endif
