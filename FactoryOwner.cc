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

#include "debug.h"

#include "util.h"
#include "FactoryOwner.h"

#include "File.h"

using namespace eda;
using namespace std;

FactoryOwner::FactoryOwner() {
  instruction_factory_ = new InstructionFactoryARM;
  instruction_factory_->InitRegisters(&memory_);
}

// These are the things the web browser can render
bool FactoryOwner::HandleGetRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() == 0) {
    //(*out) += "<html><head><title>EDA</title></head><body>I am an EDA backend, go talk to a frontend</body></html>";
    File::ReadFileToString(kDataDirectory + "backend.html", out);
  } else if(argv[0] == "favicon.ico") {
    File::ReadFileToString(kDataDirectory + argv[0], out);
  } else if(argv[0] == "Data" && argv.size() >= 2) {
    File::ReadFileToString(kDataDirectory + argv[1], out);
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
  if(argv[0] == "Address" && argv.size() >= 3) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    if (a != 0) {
      if(argv[2] == "Name") {
        (*out) += a->get_name();
      } else if(argv[2] == "Owned") {
        ostringstream ss;
        ss << kXMLHeader;
        SerializeToXML(ss, memory_.history_.get_owned(a), "owned", "number");
        (*out) += ss.str();
      } else if(argv[2] == "Xrefs") {
        ostringstream ss;
        ss << kXMLHeader;
        SerializeToXML(ss, memory_.history_.get_xrefs(a), "xrefs", "number");
        (*out) += ss.str();
      } else if(argv[2] == "Instruction") {
        ostringstream ss;
        ss << kXMLHeader;
        if(a->get_instruction() == NULL) {  // Auto-Disassembly
          instruction_factory_->Process(a);
          LOG(INFO) << "Disassembled";
        }
        if(a->get_instruction() != NULL && argv.size() >= 4) {
          if(argv[3]=="Parsed")
            a->get_instruction()->parsed_->SerializeToXML(ss);
          else if(argv[3] == "StatelessChangelist")
            a->get_instruction()->change_->SerializeToXML(ss);
        } else {
          ss << "<failure></failure>";
        }
        (*out) += ss.str();
      } else {
        return false;
      }
    } else {
      LOG(INFO) << "Address not found";
      return false;
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
  }
  return true;
}

bool FactoryOwner::HandleEvalRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() > 0) {
    ostringstream ss;
    ss << std::hex << memory_.ResolveToNumber(0, argv[0]);
    (*out) = ss.str();
  } else {
    return false;
  }
  return true;
}

bool FactoryOwner::HandleStepRequest(const std::vector<string>& argv, std::string* out) {
  if(argv[0] == "Address" && argv.size() >= 2) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    if (a != 0) {
      if(a->get_instruction() == NULL) {
        instruction_factory_->Process(a);
        LOG(INFO) << "Disassembled";
      }

      Changelist* c = changelist_factory_.CreateFromStatelessChangelist(a, *(a->get_instruction()->change_), &memory_);
      memory_.Commit(c);

      ostringstream ss;
      ss << kXMLHeader;
      c->SerializeToXML(ss);
      (*out) = ss.str();
    }
  }
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
