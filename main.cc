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

#include "debug.h"

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
  INFO << "changelist " << c->get_changelist_number() << " created, "<< c->get_size() << endl;
  m->Commit(c);
  INFO << "committed" << endl;
}

int main(int argc, char* argv[]) {
  cf = new ChangelistFactory();
  m = new Memory();
  me = m->AllocateSegment("me", 4);   // Create the `me` address, 4 is just to prevent crashing
  load_file("bootrom", 0x400000);

  while (1) {
    string cmd;
    cout << "EDA> ";
    getline(cin, cmd);

    if(cmd.substr(0,2) == "cl") {
      StatelessChangelist statelesscl;
      while (1) {
        cout << "cl> ";
        getline(cin, cmd);
        if(cmd == "") break;
        string ls = cmd.substr(0, cmd.find_first_of('='));
        string rs = cmd.substr(cmd.find_first_of('=')+1);
        statelesscl.add_change(ls, 32, "1", rs);
      }
      if(statelesscl.get_size() > 0) {
        Changelist* c = cf->CreateFromStatelessChangelist(me, statelesscl, m);
        //DebugPrint(c);
        m->Commit(c);
      }
    } else if(cmd.substr(0,7) == "printcl") {
      DebugPrint(m->history_.get_changelist(stoi(cmd.substr(8))));
    } else if(cmd.substr(0,7) == "history") {
      Address* a = m->ResolveToAddress(0, cmd.substr(8));
      if (a == NULL) {
        cout << "address not found" << endl;
        continue;
      }
      vector<int>* changes = m->history_.get_modified(a);
      if (changes == NULL) {
        cout << "no history yet" << endl;
        continue;
      }
      cout << "got modifiers "  << changes->size() << endl;
      for (vector<int>::iterator it = changes->begin(); it != changes->end(); ++it) {
        uint32_t t;
        a->get32(*it, &t);
        cout << (*it) << ": " << t << endl;
      }
    } else if(cmd.substr(0,3) == "new") {
      string segname = cmd.substr(cmd.find_first_of(' ')+1);
      m->AllocateSegment(segname, 4);
      cout << "segment " << segname << " created" << endl;
    } else if(cmd.substr(0,4) == "quit") {
      return 0;
    } else {
      uint32_t output = m->ResolveToNumber(0, cmd);
      cout << std::hex << "0x" << output << endl;
    }
  }
}
