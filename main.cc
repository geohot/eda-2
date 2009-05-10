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

Address* me;

void load_file(const string& filename, uint32_t address) {
  INFO << "loading file, " << filename << endl;
  string data;
  if(!file_to_string(filename, &data)) {
    LOG << "File read error" << endl;
  }
  INFO << "file read, " << data.size() << endl;
  m->AllocateSegment(address, data.size());
  INFO << "segment allocated" << endl;
  Changelist* c = cf->CreateFromInput(me, data, m->get_address_by_location(address));
  INFO << "changelist " << c->get_changelist_number() << " created, "<< c->size() << endl;
  m->Commit(c);
  INFO << "committed" << endl;
}

int main(int argc, char* argv[]) {
  cf = new ChangelistFactory();
  m = new Memory();
  me = new Address();
  load_file("bootrom", 0x400000);

  while (1) {
    string cmd;
    cout << "EDA> ";
    getline(cin, cmd);

    uint32_t output = m->ResolveToNumber(999, cmd);

    cout << std::hex << "0x" << output << endl;
  }
}
