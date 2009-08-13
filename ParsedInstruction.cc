// ParsedInstruction.cc -- May 16, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data.h"
#include "util.h"

#include "data_memory.h"

#include "JSON.h"

#include <map>
#include <string>

using namespace std;
using namespace eda;

map<char, string> ParsedInstruction::web_lookup_;

void ParsedInstruction::Init() {
  if(web_lookup_.empty()) {
    web_lookup_.insert(make_pair('O', "opcode"));
    web_lookup_.insert(make_pair('o', "subopcode"));
    web_lookup_.insert(make_pair('F', "flags"));
    web_lookup_.insert(make_pair('C', "flags"));
    web_lookup_.insert(make_pair('R', "register"));
    web_lookup_.insert(make_pair('I', "immed"));
    web_lookup_.insert(make_pair('S', "immed"));
    web_lookup_.insert(make_pair('P', "location"));
    web_lookup_.insert(make_pair('p', "immed"));
  }
}

// O -- opcode
// o -- subopcode
// F -- flags
// C -- condition
// R -- register
// I -- immed
// S -- signed immed
// P -- PC offset immed
// p -- dereferenced PC offset immed

string ParsedInstruction::GetConsoleString() {
  string out;
  int vpos = 0;
  for (int i = 0; i < format_.length(); i++) {
    switch(format_[i]) {
      case 'O':
      case 'o':
      case 'F':
      case 'C':
      case 'R':
      case 'I':
      case 'S':
      case 'P':
      case 'p':
        if(vpos >= args_.size()) {
          LOG(INFO) << "Error in parsed string \"" << format_ << "\" size is only " << args_.size();
          break;
        }
        out += args_[vpos];
        vpos ++;
        break;
      default:
        out += format_[i];
    }
  }
  return out;
}


void ParsedInstruction::SerializeToXML(ostringstream& out) {
  out << "<ParsedInstruction>";
  int vpos = 0;
  bool lastformatting = false;
  for (int i = 0; i < format_.length(); i++) {
    map<char, string>::iterator it = web_lookup_.find(format_[i]);
    if(it != web_lookup_.end()) {
      if(vpos >= args_.size()) {
        LOG(INFO) << "Error in parsed string \"" << format_ << "\" size is only " << args_.size();
        break;
      }
      if(lastformatting==true) {
        out << "</formatting>";
        lastformatting = false;
      }
      if(args_[vpos].length() != 0) {
        out << "<atom>";
        out << "<type>" << it->second << "</type>";
        if(format_[i] == 'P') {
          Address* target = parent_->memory_->ResolveToAddress(0, immed(parent_->get_location())+"+"+args_[vpos]);
          if(target != NULL)
            out << "<data>" << target->get_name() << "</data>";
          else
            out << "<data>" << parent_->memory_->ResolveToNumber(0, immed(parent_->get_location())+"+"+args_[vpos]) << "</data>";
        } else
          out << "<data>" << args_[vpos] << "</data>";
        out << "</atom>";
      }
      vpos++;
    } else {
      if(lastformatting==false) {
        out << "<formatting>";
        lastformatting = true;
      }
      out << format_[i];
    }
  }
  if(lastformatting==true) {
    out << "</formatting>";
    lastformatting = false;
  }

  out << "</ParsedInstruction>";
}


void ParsedInstruction::SerializeToJSON(JSON* json) {
  vector<JSON> atoms;
  int vpos = 0;
  bool lastformatting = false;
  string format = "";
  for (int i = 0; i < format_.length(); i++) {
    JSON atom;
    map<char, string>::iterator it = web_lookup_.find(format_[i]);
    if(it != web_lookup_.end()) {
      LOG(DEBUG) << "arg(" << vpos << "): " << args_[vpos];
      if(lastformatting == true) {
        JSON formatj;
        formatj.add("type", "formatting");
        formatj.add("data", format);
        format = "";
        atoms.push_back(formatj);
        lastformatting = false;
      }
      if(vpos >= args_.size()) {
        LOG(INFO) << "Error in parsed string \"" << format_ << "\" size is only " << args_.size();
        break;
      }
      atom.add("type", it->second);
      if(format_[i] == 'P') {
        LOG(INFO) << "got location: " << immed(parent_->get_location()) << "+" << args_[vpos];
        Address* target = parent_->memory_->ResolveToAddress(0, immed(parent_->get_location())+"+"+args_[vpos]);
        if(target != NULL)
          atom.add("data", target->get_name());
        else
          atom.add("data", parent_->memory_->ResolveToNumber(0, immed(parent_->get_location())+"+"+args_[vpos]));
      } else {
        atom.add("data", args_[vpos]);
      }
      atoms.push_back(atom);
      vpos++;
    }
    else {
      format += format_[i];
      lastformatting = true;
    }
  }
  if(lastformatting == true) {
    JSON formatj;
    formatj.add("type", "formatting");
    formatj.add("data", format);
    format = "";
    atoms.push_back(formatj);
    lastformatting = false;
  }
  
  if(vpos != args_.size()) {
    LOG(WARNING) << "Not all args were used in " << format_ << " length was " << args_.size();
  }
  
  json->add("ParsedInstruction", atoms);
}
