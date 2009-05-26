// main.cc -- May 9, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

// This EDA is just focused on being a simulator first
// No threading, no servers, no bullshit

#include <iostream>
#include <iomanip>
#include <string>

#include "logic.h"
#include "data_memory.h"
#include "util.h"

#include "Servlet.h"
#include "FactoryOwner.h"

#include "debug.h"

#include "InstructionFactoryARM.h"

using namespace std;
using namespace eda;

ChangelistFactory* cf;
Memory* m;

Address* me;

void load_file(const string& filename, uint32_t address) {
  LOG(INFO) << "loading file, " << filename;
  string data;
  if(!file_to_string(filename, &data)) {
    LOG(WARNING) << "File read error";
  }
  LOG(INFO) << "file read, " << data.size();
  m->AllocateSegment(address, data.size());
  LOG(INFO) << "segment allocated";
  Changelist* c = cf->CreateFromInput(me, data, m->get_address_by_location(address));
  LOG(INFO) << "changelist " << c->get_changelist_number() << " created, "<< c->get_size();
  m->Commit(c);
  LOG(INFO) << "committed";
}

Servlet<FactoryOwner> s;
FactoryOwner f;

void quitproc(int a) {
  LOG(INFO) << "Exitting normally";
  s.EndServer();
  return;
}

int main(int argc, char* argv[]) {
  signal(SIGHUP, quitproc);
  signal(SIGINT, quitproc);
  signal(SIGQUIT, quitproc);

  s.RegisterCommandHandler("GET", &f, &FactoryOwner::HandleGetRequest);
  s.StartServer(8080);

  return 0;
}

int frontend_console() {
  cf = new ChangelistFactory();
  m = new Memory();
  me = m->AllocateSegment("me", 4);   // Create the `me` address, 4 is just to prevent crashing
  load_file("bootrom", 0x400000);

  m->AllocateSegment(0xf4300000, 0x100);
  m->AllocateSegment(0xf4400000, 0x100);

  InstructionFactoryARM iarm;
  iarm.InitRegisters(m);
  Address* PC = m->ResolveToAddress(0,"`PC`");

  //INFO << "got PC" << endl;
  PC->set32(1, 0x400008);

  Address* next_disassembly_address;

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
        statelesscl.add_change(ls, "1", 4, rs);
      }
      if(statelesscl.get_size() > 0) {
        DebugPrint(&statelesscl);
        Changelist* c = cf->CreateFromStatelessChangelist(me, statelesscl, m);
        //DebugPrint(c);
        m->Commit(c);
      }
    } else if(cmd.substr(0,7) == "printcl") {
      DebugPrint(m->history_.get_changelist(stoi(cmd.substr(8))));
    } else if(cmd.substr(0,1) == "g") {   // Go
      Address* a;

      while(1) {
        a = m->ResolveToAddress(0, "[`PC`]-8");   // hardcoded for now
        if(a->get_instruction() == NULL) {
          next_disassembly_address = iarm.Process(a);
        } //else
          //break;    //been here before

        cout << "******* " << a->get_name() << endl;
        cout << "******* ";
        DebugPrint(a->get_instruction()->parsed_);
        if (a->get_instruction()->change_->get_size() == 1) {
          cout << "*******UNIMPLEMENTED INSTRUCTION" << endl;
        }
        DebugPrint(a->get_instruction()->change_);
        cout << "*******" << endl;

        Changelist* c = cf->CreateFromStatelessChangelist(a, *(a->get_instruction()->change_), m);
        DebugPrint(c);
        m->Commit(c);
      }


    } else if(cmd.substr(0,1) == "r") {
      Address* a;
      if(cmd.length() > 1)
        a = m->ResolveToAddress(0, cmd.substr(cmd.find_first_of(' ')+1));
      else
        a = m->ResolveToAddress(0, "[`PC`]-8");

      if(a->get_instruction() == NULL) {
        next_disassembly_address = iarm.Process(a);
      }

      cout << "******* " << a->get_name() << endl;
      cout << "******* ";
      DebugPrint(a->get_instruction()->parsed_);
      DebugPrint(a->get_instruction()->change_);
      cout << "*******" << endl;

      Changelist* c = cf->CreateFromStatelessChangelist(a, *(a->get_instruction()->change_), m);
      DebugPrint(c);
      m->Commit(c);
    } else if(cmd.substr(0,1) == "d") {
      Address* a;
      if(cmd.length() > 1)
        a = m->ResolveToAddress(0, cmd.substr(cmd.find_first_of(' ')+1));
      else {
        a = next_disassembly_address;
      }
      next_disassembly_address = iarm.Process(a);
      cout << a->get_name() << endl;
      DebugPrint(a->get_instruction()->parsed_);
      DebugPrint(a->get_instruction()->change_);
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
        cout << m->history_.get_changelist(*it)->get_owner()->get_name() << " ";
        cout << setfill(' ') << dec << setw(3) << (*it) << ": " << hex << t << endl;
      }
    } else if(cmd.substr(0,3) == "new") {
      string segname = cmd.substr(cmd.find_first_of(' ')+1);
      m->AllocateSegment(segname, 4);
      cout << "segment " << segname << " created" << endl;
    } else if(cmd.substr(0,1) == "q") {
      return 0;
    } else if(cmd.substr(0,1) == "c") {
      int clnum = stoi(cmd.substr(1));
      uint32_t output = m->ResolveToNumber(clnum, cmd.substr(cmd.find(' ')+1));
      cout << std::hex << "0x" << output << " at cl " << dec << clnum << endl;
    } else {
      uint32_t output = m->ResolveToNumber(0, cmd);
      cout << std::hex << "0x" << output << endl;
    }
  }
}
