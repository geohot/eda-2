// util.h -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

namespace eda {

// Returns things like 9 and 0x4F
string immed(uint32 data);

inline uint32 rol(uint32 data, int len);
inline uint32 ror(uint32 data, int len);

}
