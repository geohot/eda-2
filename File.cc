// File.cc -- May 21, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "File.h"

using namespace eda;
using namespace std;

bool WriteFileFromString(const std::string& filename, const std::string& data) {
  FILE *f = fopen(filename.c_str(), "wb");
  if (f == 0) return false;
  fwrite(data.data(), 1, data.size(), f);
  fclose(f);
  return true;
}

bool ReadFileToString(const std::string& filename, std::string* data) {
  FILE *f = fopen(filename.c_str(), "rb");
  if (f == 0) return false;
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);
  // There should be a faster way to do this, can I read to string.data()?
  char* d = (char*)malloc(size);
  bool ret = false;
  if (fread(d, 1, size, f) == size) {
    ret = true;
    data.append(d, size);
  }
  fclose(f);
  delete d;
  return ret;
}
