// FactoryOwner.cc -- May 24, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// FactoryOwners run the EDA show


// /Address/<expression>/
//    Name
//    Changelists(Changelists that it created)
//    Xrefs(Changelists that modified it)
//    Instruction/Parsed
//    Instruction/StatelessChangelist

// /Changelist/<number>

// provided for debugging only, or not
// /Eval/

#include <iomanip>

#include "data.h"
#include "debug.h"

#include "util.h"
#include "FactoryOwner.h"

#include "InstructionFactoryISDF.h"

#include "File.h"

#include "JSON.h"

using namespace eda;
using namespace std;

FactoryOwner::FactoryOwner() {
  //instruction_factory_ = new InstructionFactoryARM;
  //instruction_factory_  = new InstructionFactoryISDF("thumb.isdf", &memory_);
  instruction_factory_  = new InstructionFactoryISDF("arm.isdf", &memory_);
  //instruction_factory_->InitRegisters(&memory_);
}

// These are the things the web browser can render
bool FactoryOwner::HandleGetRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() == 0) {
    //(*out) += "<html><head><title>EDA</title></head><body>I am an EDA backend, go talk to a frontend</body></html>";
    File::ReadFileToString(kDataDirectory + "index.html", out);
  } else if(argv[0] == "favicon.ico") {
    File::ReadFileToString(kDataDirectory + argv[0], out);
  } else if(argv[0] == "Data" && argv.size() >= 2) {
    string filename = kDataDirectory + argv[1];
    for(int i = 2; i < argv.size(); i++) filename += "/" + argv[i];
    File::ReadFileToString(filename, out);
  } else
    (*out) += "<html><head><title>EDA</title></head><body><h1>Resource not found</h1></body></html>";
  return true;
}

// This is where the web browser sends things, like files
bool FactoryOwner::HandlePostRequest(const std::vector<string>& argv, std::string* out) {
  return true;
}

// Read because a browser can't see these
bool FactoryOwner::HandleReadRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() == 0)
    return false;
  JSON data;
  if(argv[0] == "Address" && argv.size() >= 2) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    /*if(a != 0 && a->get_instruction() == NULL) {  // Auto-Disassembly
      instruction_factory_->Process(a);
      LOG(INFO) << "Disassembled";
    }*/
    if (a != 0 && argv.size() >= 3) {
      if(argv[2] == "Name") {
        data.add("Name", a->get_name());
      } else if(argv[2] == "Owned") {
        data.add("Owned", *(memory_.history_.get_owned(a)));
      } else if(argv[2] == "Xrefs") {
        data.add("Xrefs", *(memory_.history_.get_xrefs(a)));
      } else if(argv[2] == "Instruction") {
        if(a->get_instruction() == NULL) {  // Auto-Disassembly
          instruction_factory_->Process(a);
          LOG(INFO) << "Disassembled";
        }
        if(a->get_instruction() != NULL && argv.size() >= 4) {
          if(argv[3]=="Parsed")
            a->get_instruction()->parsed_->SerializeToJSON(&data);
          else if(argv[3] == "StatelessChangelist")
            a->get_instruction()->change_->SerializeToJSON(&data);
        } else if(a->get_instruction() != NULL) {
          a->get_instruction()->SerializeToJSON(&data);
        }
      } else {
        return false;
      }
    } else if(a != 0) {
      /*ostringstream ss;
      ss << kXMLHeader;
      LOG(DEBUG) << "making xml";
      a->SerializeToXML(ss);
      LOG(DEBUG) << "done making xml";
      (*out) += ss.str();*/
      a->SerializeToJSON(&data);
    } else {
      LOG(INFO) << "Address not found";
      return false;
    }
    (*out) = data.serialize();
  } /*else if(argv[0] == "Function" && argv.size() >= 2) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    if(a != NULL && a->get_instruction() != NULL) {
      set<Address*> addresses;
      a->get_instruction()->GetFunction(&addresses);
      ostringstream ss;
      ss << kXMLHeader;
      ss << "<Function>";
      if(argv.size() >= 3 && argv[2] == "List") {
        for(set<Address*>::iterator it = addresses.begin(); it != addresses.end(); ++it) {
          ss << std::hex << "<location>" << (*it)->get_location() << "</location>";
        }
      } else {
        for(set<Address*>::iterator it = addresses.begin(); it != addresses.end(); ++it) {
          (*it)->SerializeToXML(ss);
        }
      }
      ss << "</Function>";
      (*out) = ss.str();
    }
  } else if(argv[0] == "Changelist" && argv.size() >= 2) {
    Changelist* c = memory_.history_.get_changelist(stoi(argv[1]));

    ostringstream ss;
    ss << kXMLHeader;
    if(c != NULL)
      c->SerializeToXML(ss);
    else
      ss << "<failure></failure>";
    (*out) = ss.str();
  } else if(argv[0] == "State") {
    ostringstream ss;
    ss << kXMLHeader;
    ss << "<State>";
    instruction_factory_->StateToXML(ss);
    ss << "<currentchangelistnumber>" << changelist_factory_.get_current_changelist_number() << "</currentchangelistnumber>";
    ss << "</State>";
    (*out) = ss.str();
  }*/ else if(argv[0] == "Memory" && argv.size() >= 4) {
    // Dump memory raw
    Address* a = memory_.get_address_by_location(stoi(argv[1]));
    int len = stoi(argv[2]);
    int clnum = stoi(argv[3]);
    ostringstream ss;

    for(int i = 0; i < len; i++) {
      uint8_t data;
      a = a->get8(clnum, &data);
      ss << setfill('0') << setw(2) << hex << (int)data;
    }
    (*out) = ss.str();
  }
  return true;
}

bool FactoryOwner::HandleEvalRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() > 0) {
    ostringstream ss;
    ss << std::hex << memory_.ResolveToNumber(0, argv[0]);
    JSON ret;
    ret.add("result", ss.str());
    (*out) = ret.serialize();
  } else {
    return false;
  }
  return true;
}

bool FactoryOwner::HandleRenameRequest(const std::vector<string>& argv, std::string* out) {
  if(argv[0] == "Address" && argv.size() >= 3) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    LOG(INFO) << "Renaming " << a->get_name() << " to " << argv[2];
    memory_.Rename(a, argv[2]);
  }
  return true;
}

bool FactoryOwner::HandleStepRequest(const std::vector<string>& argv, std::string* out) {
  if(argv[0] == "Address" && argv.size() >= 2) {
    Address* stop;
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    if(argv.size() == 3) { // Run until
      stop = memory_.ResolveToAddress(0, argv[2]);
      while (a != stop) {
        if(a->get_instruction() == NULL) instruction_factory_->Process(a);
        Changelist* c = changelist_factory_.CreateFromStatelessChangelist(a, *a->get_instruction()->change_, &memory_);
        memory_.Commit(c);
        a = memory_.ResolveToAddress(0, argv[1]);
      }
    } else {
      if (a != 0) {
        if(a->get_instruction() == NULL) {
          instruction_factory_->Process(a);
          LOG(INFO) << "Disassembled";
        }
        StatelessChangelist* slcl = a->get_instruction()->change_;
        DebugPrint(slcl);
        LOG(DEBUG) << "got address, creating changelist";
        Changelist* c = changelist_factory_.CreateFromStatelessChangelist(a, *slcl, &memory_);
        LOG(DEBUG) << "changelist created";
        memory_.Commit(c);

        /*ostringstream ss;
        ss << kXMLHeader;
        c->SerializeToXML(ss);
        (*out) = ss.str();*/
        
        JSON ret;
        c->SerializeToJSON(&ret);
        (*out) = ret.serialize();
      }
    }
  }
  return true;
}


bool FactoryOwner::HandleDisassembleRequest(const std::vector<string>& argv, std::string* out) {
  if(argv[0] == "Address" && argv.size() >= 2) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    if (a != 0) {
      if(a->get_instruction() == NULL) {
        instruction_factory_->Process(a);
        LOG(INFO) << "Disassembled";
      }
      /*LOG(INFO) << a->get_name();
      DebugPrint(a->get_instruction()->parsed_);
      DebugPrint(a->get_instruction()->change_);*/

      //a->get_instruction()->change_->SerializeToXML(ss);
      //(*out) += ss.str();
      (*out) = "Done";
    } else {
      LOG(INFO) << "run address not found";
      return false;
    }
  }
  return true;
}
