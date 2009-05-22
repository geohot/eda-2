// File.h -- May 21, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// File needs two methods, and only two methods
// If your File has more than two methods, ask for a refund


#include <string>
#include <cstdlib>

namespace eda {

class File {
public:
  static bool WriteFileFromString(const std::string& filename, const std::string& data);
  static bool ReadFileToString(const std::string& filename, std::string* data);
};

}
