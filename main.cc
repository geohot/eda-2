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

#include "InstructionFactoryISDF.h"

#include "JSON.h"

using namespace std;
using namespace eda;

void load_file(Memory *m, ChangelistFactory *cf, Address* me, const string& filename, uint32_t address) {
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

void quitproc(int a) {
  LOG(INFO) << "Exiting normally";
  s.EndServer();
  exit(0);
}

int main(int argc, char* argv[]) {
  LOG(INFO) << "eda started";
  
  
  /*JSON test;
  
  test.add("hello", 1);
  test.add("goodbye", "world");
  
  vector<int> a;
  a.push_back(1);
  a.push_back(2);
  
  test.add("arr", a);
  
  LOG(INFO) << test.serialize();
  
  return 0;*/
  
  //LOG(INFO) << "Servlet constructed";
  FactoryOwner f;
  LOG(INFO) << "FactoryOwner constructed";
#ifndef WIN32
  signal(SIGHUP, quitproc);
  signal(SIGINT, quitproc);
  signal(SIGQUIT, quitproc);
#endif
  LOG(INFO) << "exit signals registered";
  Address* me = f.memory_.AllocateSegment("me", 4);   // Create the `me` address, 4 is just to prevent crashing

  
  LOG(INFO) << "me address allocated";
  /*Address* test = f.memory_.AllocateSegment("test", 8);
  test->set32(1, 0x465E);
  InstructionFactoryISDF a("thumb.isdf", &f.memory_);
  a.Process(test);*/


  load_file(&f.memory_, &f.changelist_factory_, me, "bootrom", 0x400000);
  //load_file(&f.memory_, &f.changelist_factory_, me, "ibec.dfu", 0x18000000);

  //f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x1800F99C);
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x400024);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x400020);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  /*f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x40001C);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));*/
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x400018);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x400014);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x400010);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x40000C);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  f.memory_.ResolveToAddress(0,"`PC`")->set32(1, 0x400008);
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 8"));
  
  /*f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x400004"));
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x400008"));
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x40000C"));
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x400010"));
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x400014"));
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x400018"));
  f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "0x40001C"));*/
  //f.instruction_factory_->FastAnalyse(&f.memory_, f.memory_.ResolveToAddress(0, "[`PC`] - 4"));

  f.memory_.AllocateSegment(0, 0x40000);

  f.memory_.ResolveToAddress(0,"`SP`")->set32(1, 0x10000);

  f.memory_.AllocateSegment(0xf4300000, 0x100);
  f.memory_.AllocateSegment(0xf4400000, 0x100);

  s.RegisterCommandHandler("GET", &f, &FactoryOwner::HandleGetRequest);
  s.RegisterCommandHandler("EVAL", &f, &FactoryOwner::HandleEvalRequest);
  s.RegisterCommandHandler("READ", &f, &FactoryOwner::HandleReadRequest);
  s.RegisterCommandHandler("STEP", &f, &FactoryOwner::HandleStepRequest);
  s.RegisterCommandHandler("RENAME", &f, &FactoryOwner::HandleRenameRequest);
  s.RegisterCommandHandler("DISASSEMBLE", &f, &FactoryOwner::HandleDisassembleRequest);
  s.StartServer(8080);

  return 0;
}

int frontend_console() {
  ChangelistFactory* cf;
  Memory* m;

  Address* me;

  cf = new ChangelistFactory();
  m = new Memory();
  me = m->AllocateSegment("me", 4);   // Create the `me` address, 4 is just to prevent crashing
  load_file(m, cf, me, "bootrom", 0x400000);

  m->AllocateSegment(0xf4300000, 0x100);
  m->AllocateSegment(0xf4400000, 0x100);

  InstructionFactoryARM iarm;
  iarm.InitRegisters(m);
  Address* PC = m->ResolveToAddress(0,"`PC`");

  //INFO << "got PC" << endl;
  PC->set32(1, 0x1800F99C);

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
      vector<int>* changes = m->history_.get_xrefs(a);
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
