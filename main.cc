// main.cc -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

// This EDA is just focused on being a simulator first
// No threading, no servers, no bullshit

#include <iostream>
#include <string>

#include "logic.h"
#include "data_memory.h"
#include "util.h"

using namespace std;
using namespace eda;

ChangelistFactory* cf;
Memory* m;

/*void load_file(const string& filename, uint32_t address) {
  string data;
  file_to_string(filename &data);
  m->AllocateSegment(address, data.size());
  Changelist* c;
  cf->CreateFromInput(data, m->get_address_by_location(address), c);
  m->Commit(c);
}*/

int main(int argc, char* argv[]) {
  //cf = new ChangelistFactory();
  m = new Memory();
  //load_file("bootrom", 0x400000);

  while (1) {
    string cmd;
    getline(cin, cmd);

    uint32_t output;
    m->Resolve(999, cmd, &output);

    cout << output << endl;
  }
}
