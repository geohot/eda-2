// FactoryOwner.cc -- May 24, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// FactoryOwners run the EDA show


// /Address/<expression>/
//    Name/
//    Modifiers/
//    Modified/

// /Changelist/<number>/

// provided for debugging only, or not
// /Eval/

#include "util.h"
#include "FactoryOwner.h"

#include "File.h"

using namespace eda;
using namespace std;

FactoryOwner::FactoryOwner() {
  instruction_factory_arm_.InitRegisters(&memory_);
}

bool FactoryOwner::HandleGetRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() == 0) {
    //(*out) += "<html><head><title>EDA</title></head><body>I am an EDA backend, go talk to a frontend</body></html>";
    File::ReadFileToString(kDataDirectory + "backend.html", out);
  } else if(argv[0] == "favicon.ico") {
    File::ReadFileToString(kDataDirectory + argv[0], out);
  } else if(argv[0] == "Data" && argv.size() >= 2) {
    File::ReadFileToString(kDataDirectory + argv[1], out);
  } else if(argv[0] == "Address" && argv.size() >= 3) {
    Address* a = memory_.ResolveToAddress(0, argv[1]);
    if (a != 0) {
      if(argv[2] == "Name") {
        (*out) += a->get_name();
      } else if(argv[2] == "Modifiers") {
        ostringstream ss;
        ss << kXMLHeader;
        SerializeToXML(ss, memory_.history_.get_modifiers(a), "modifiers", "number");
        (*out) += ss.str();
      } else if(argv[2] == "Modified") {
        ostringstream ss;
        ss << kXMLHeader;
        SerializeToXML(ss, memory_.history_.get_modified(a), "modified", "number");
        (*out) += ss.str();
      } else {
        (*out) += "<html><head><title>EDA</title></head><body><h1>Resource not found</h1></body></html>";
      }
    } else {
      LOG(INFO) << "Address not found";
    }
  }
  else
    (*out) += "<html><head><title>EDA</title></head><body><h1>Resource not found</h1></body></html>";
  return true;
}

bool FactoryOwner::HandleEvalRequest(const std::vector<string>& argv, std::string* out) {
  if(argv.size() > 0) {
    ostringstream ss;
    ss << std::hex << memory_.ResolveToNumber(0, argv[0]);
    (*out) = ss.str();
  }
}
