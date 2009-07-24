// Instruction.cc -- May 29, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "data.h"
#include "util.h"

#include "JSON.h"

using namespace eda;
using namespace std;

void Instruction::SerializeToXML(ostringstream& out) {
  out << std::hex << "<Instruction>";
  parsed_->SerializeToXML(out);
  //LOG(DEBUG) << "    wrote parsed";
  change_->SerializeToXML(out);
  //LOG(DEBUG) << "    wrote change";
  out << "<metadata>";
  for(vector<Address*>::iterator it = control_inputs_.begin(); it != control_inputs_.end(); ++it) {
    out << "<input>0x" << (*it)->get_location() << "</input>";
  }
  for(vector<Address*>::iterator it = control_outputs_.begin(); it != control_outputs_.end(); ++it) {
    out << "<output>0x" << (*it)->get_location() << "</output>";
  }
  for(vector<Address*>::iterator it = control_indirect_inputs_.begin(); it != control_indirect_inputs_.end(); ++it) {
    out << "<input_indirect>0x" << (*it)->get_location() << "</input_indirect>";
  }
  for(vector<Address*>::iterator it = control_indirect_outputs_.begin(); it != control_indirect_outputs_.end(); ++it) {
    out << "<output_indirect>0x" << (*it)->get_location() << "</output_indirect>";
  }
  out << "</metadata>";
  out << "</Instruction>";
}

void Instruction::SerializeToJSON(JSON* json) {
  JSON is;
  parsed_->SerializeToJSON(&is);
  change_->SerializeToJSON(&is);
  is.add("input", FetchGAIsFromAddresses(control_inputs_));
  is.add("output", FetchGAIsFromAddresses(control_outputs_));
  is.add("input_indirect", FetchGAIsFromAddresses(control_indirect_inputs_));
  is.add("input_indirect", FetchGAIsFromAddresses(control_indirect_outputs_));
  json->add("Instruction", is);
}

void Instruction::GetFunction(set<Address*>* addresses) {
  if(addresses->insert(start_).second) {
    for(vector<Address*>::iterator it = control_inputs_.begin(); it != control_inputs_.end(); ++it) {
      if((*it)->get_instruction() != NULL)
        (*it)->get_instruction()->GetFunction(addresses);
    }
    for(vector<Address*>::iterator it = control_outputs_.begin(); it != control_outputs_.end(); ++it) {
      if((*it)->get_instruction() != NULL)
        (*it)->get_instruction()->GetFunction(addresses);
    }
  }
}
