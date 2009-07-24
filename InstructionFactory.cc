// InstructionFactory.cc -- June 3, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// This has the fast analyser

#include "logic.h"
#include "data.h"

#include "util.h"

using namespace eda;
using namespace std;

// This is the fast analyser
// It traverses every codepath
void InstructionFactory::FastAnalyse(Memory* m, Address* start) {
  //Memory temp;
  //Address* temp_program_counter = temp.AllocateSegment(program_counter_->get_name(), 4);
  //temp_program_counter->set32(1, TranslateFromProgramCounter(start->get_location()));
  uint32_t start_location = start->get_location();
  int changelist_number = 1;
  Process(start);
  FastAnalyseRecurse(m, start, program_counter_, &changelist_number);

  program_counter_->Clear32();
  program_counter_->set32(1, TranslateFromProgramCounter(start_location));
}

// Should be private
void InstructionFactory::FastAnalyseRecurse(Memory* m, Address* location, Address* temp_program_counter, int* changelist_number) {
  LOG(INFO) << std::hex << "Recursing 0x"  << location->get_location() << " on change " << (*changelist_number);
  Instruction* this_instruction = location->get_instruction();
  StatelessChangelistIterator change;
// Find LR Stuff
  vector<StatelessChangelistIterator> pc_changes;
  string lr_condition = "";
  int static_changelist_number = (*changelist_number);
  if(!this_instruction->change_->get_first_change(&change)) {
    LOG(INFO) << "get changes fail";
    return;
  }
  do {
    Address* target = m->ResolveToAddress(0, change->first.first);
    if(target == link_register_) {
      lr_condition = change->first.second;
      if(lr_condition == "1") {   // if the linked branch always happens
        pc_changes.push_back(change); //add the link register change to the PC changes
      }
    } else if(target == program_counter_) {
      pc_changes.push_back(change);
    }
  } while(this_instruction->change_->get_next_change(&change));

// Do PC Stuff
  for(int i = 0; i < pc_changes.size(); i++) {
    // Check to see if this change targets the program counter
    //LOG(INFO) << std::hex << "branch of " << location->get_location();
    //LOG(INFO) << "is program counter";
    // If so, evaluate it in this frame
    uint32_t next_pc = m->ResolveToNumber(static_changelist_number, pc_changes[i]->second.second);
    if(next_pc == 0xFFFFFFF) continue;    // This is a hack
    temp_program_counter->set32(++(*changelist_number), next_pc);
    ostringstream o; o << "0x" << std::hex << TranslateToProgramCounter(next_pc);
    //LOG(INFO) << "targetting " << o.str();
    Address* next_address = m->ResolveToAddress(0, o.str());
    if(next_address != NULL) {
      if(m->ResolveToAddress(0, pc_changes[i]->first.first) == link_register_) {    // This is the lr return address
        if(next_address->get_instruction() == NULL) {
          Process(next_address);
          FastAnalyseRecurse(m, next_address, temp_program_counter, changelist_number);
        }
      } else if(pc_changes[i]->first.second == lr_condition) {    // This is a linked branch
        this_instruction->control_indirect_outputs_.push_back(next_address);
        if(next_address->get_instruction() == NULL) {
          Process(next_address);
          next_address->get_instruction()->control_indirect_inputs_.push_back(location);
          FastAnalyseRecurse(m, next_address, temp_program_counter, changelist_number);
        } else {
          next_address->get_instruction()->control_indirect_inputs_.push_back(location);
        }
      } else {    // This is direct
        this_instruction->control_outputs_.push_back(next_address);
        if(next_address->get_instruction() == NULL) {
          Process(next_address);
          next_address->get_instruction()->control_inputs_.push_back(location);
          FastAnalyseRecurse(m, next_address, temp_program_counter, changelist_number);
        } else {
          next_address->get_instruction()->control_inputs_.push_back(location);
        }
      }
    } else {
      LOG(INFO) << "invalid address: " << o.str();
    }
  }
}
