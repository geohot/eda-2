// util.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#ifndef EDA_UTIL_H_
#define EDA_UTIL_H_

#include <iostream>
#include <string>

namespace eda {

// Returns things like 9 and 0x4F
std::string immed(uint32_t data);
// Signed
std::string immed_signed(int32_t data);

uint32_t stoi(const std::string& num);

bool file_to_string(const std::string& filename, std::string* out);

// LOG(INFO)
// LOG(ERROR)

#ifdef WIN32
#define THIS_FILE ((strrchr(__FILE__, 0x5C) ?: __FILE__ - 1) + 1)
#else
#define THIS_FILE ((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)
#endif

//#define LOG std::cout << std::endl << std::hex << THIS_FILE << "--" << __PRETTY_FUNCTION__ << ": "
//#define LOG std::cout << std::dec << THIS_FILE << "--" << __PRETTY_FUNCTION__ << ": "
#define LOG std::cout << std::dec << THIS_FILE << "--" << __FUNCTION__  << ": "
#define INFO std::cout << std::dec << THIS_FILE << ": "

//inline uint32_t rol(uint32_t data, int len);
#define rol(data, len) (data << len) | (data >> (32-len))
//inline uint32_t ror(uint32_t data, int len);
#define ror(data, len) (data >> len) | (data << (32-len))

}

#endif
