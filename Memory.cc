// Memory.cc -- May 10, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include <vector>

#include "data_memory.h"
#include "util.h"

namespace eda {

void Memory::AllocateSegment(uint32_t address_32, int length) {
  vector<Address*>* ts = AllocateSegment(length);
  space_.insert(make_pair(address_32, ts));
}

void Memory::AllocateSegment(const string& name, int length) {
  vector<Address*>* ts = AllocateSegment(length);
  (*ts)[0]->set_name(name);
  named_.insert(make_pair(name, (*ts)[0]));
}

vector<Address*>* Memory::AllocateSegment(int length) {
  vector<Address*>* ts = new vector<Address*>(length);

  for (int l = 0; l < length; l++) {
    (*ts)[l] = new Address();
  }

  // Setup the next pointers
  for (int l = 0; l < length-1; l++) {
    (*ts)[l]->set_next((*ts)[l+1]);
  }

  segments_.push_back(ts);  // Add it to the segment list
  return ts;
}

Address* Memory::get_address_by_location(uint32_t address_32) {
  map<uint32_t, vector<Address*>*>::iterator a = space_.upper_bound(address_32);
  if(a == space_.begin()) return NULL;
  --a;
  if(a->first + a->second->size() > address_32)
    return (*a->second)[address_32 - a->first];
  else
    return NULL;
}

Address* Memory::get_address_by_name(const string& name) {
  map<string, Address*>::iterator a = named_.find(name);
  if(a == named_.end())
    return NULL;
  else
    return a->second;
}

enum {
  OPER_ADD,
  OPER_SUB,
  OPER_MUL,
  OPER_DIV,
  OPER_LSL,
  OPER_LSR,
  OPER_ASR,
  OPER_ROR,
  OPER_ROL,
};

// Recursive function for resolving stateless strings
// Should really return a vector of things it accessed too
uint32_t Memory::ResolveToNumber(int changelist_number, const string& stateless) {
  INFO << "resolving " << stateless << endl;
  // Segments are [..], (..), and `..`
  //   [..] is get_address_by_location with implied (..) and deref
  //   (..) is ResolveToNumber
  //   `..` is get_address_by_name and deref i.e. `R2` is the value in R2
  // Operators are + - * / << >> >>> >/>
  // - is a special operator
  // Numbers can just exist or start with 0x
  // Whitespace is ignored
  int string_location = 0, next_string_location;
  uint32_t lastval, retval=0;
  int oper = OPER_ADD;
  bool error = false;
  Address* addr;
  while(error == false && string_location < stateless.length()) {
    bool operate = false;
    // Skip whitespace
    while (stateless[string_location] == ' ') string_location++;

    next_string_location = string_location;

    switch (stateless[string_location]) {
      case '[':
        next_string_location = stateless.find(']', string_location);
        addr = ResolveToAddress(changelist_number, stateless.substr(string_location+1, next_string_location-string_location-1));
        if(addr != NULL) {
          addr->get32(changelist_number, &lastval);
          operate = true;
        }
        else {
          LOG << "memory not found: " << stateless.substr(string_location+1, next_string_location-string_location-1) << endl;
          error = true;
        }
        break;
      case '(':
        next_string_location = stateless.find(')', string_location);
        lastval = ResolveToNumber(changelist_number, stateless.substr(string_location+1, next_string_location-string_location-1));
        operate = true;
        break;
      case '`':
        // This can't resolve addresses
        LOG << "can't resolve addresses" << endl;
        error = true;
        break;
      case '+': oper = OPER_ADD; break;
      case '-': oper = OPER_SUB; break;
      case '*': oper = OPER_MUL; break;
      case '/': oper = OPER_DIV; break;
      case '<':
      case '>':
        if (stateless[++next_string_location] == '<') {
          oper = OPER_LSL; break;
        } else if(stateless[next_string_location] == '>') {
          if(stateless[next_string_location+1] == '>') {
            ++next_string_location;
            oper = OPER_ASR; break;
          } else {
            oper = OPER_LSR; break;
          }
        } else if(stateless[next_string_location] == '/' && stateless[++next_string_location] == '>') {
          oper = OPER_ROR; break;
        } else if(stateless[next_string_location] == '<') {
          oper = OPER_ROL; break;
        } else {
          LOG << "unknown operator" << endl;
          error = true;
          break;
        }
      default:    // Numbers
        while(stateless.size() != next_string_location &&
               ((stateless[next_string_location] >= '0' && stateless[next_string_location] <= '9') ||
                (stateless[next_string_location] >= 'a' && stateless[next_string_location] <= 'f') ||
                (stateless[next_string_location] >= 'A' && stateless[next_string_location] <= 'F') ||
                 stateless[next_string_location] == 'x')) {
          next_string_location++;
        }
        if(next_string_location != string_location) {
          lastval = stoi(stateless.substr(string_location, next_string_location-string_location));
          next_string_location--;
          operate = true;
        }
        else {
          error = true;
          LOG << "bad character" << endl;
        }
        break;
    }

    // Run the operation
    if (operate) {
      //INFO << "EVAL: " << retval << " " << oper << " " << lastval << endl;
      switch (oper) {
        case OPER_ADD: retval+=lastval; break;
        case OPER_SUB: retval-=lastval; break;
        case OPER_MUL: retval*=lastval; break;
        case OPER_DIV: retval/=lastval; break;
        case OPER_LSL: retval<<=lastval; break;
        case OPER_LSR: retval>>=lastval; break;
        case OPER_ASR: retval>>=lastval; break;
        case OPER_ROL: retval=rol(retval, lastval); break;
        case OPER_ROR: retval=ror(retval, lastval); break;
        default:
          error = true;
      }
    }

    string_location = next_string_location+1;
  }
  if(error == true) {
    LOG << "Error in parser: " << stateless << "[" << string_location << "]" << endl;
  }
  INFO << "got " << std::hex << retval << endl;
  return retval;
}

Address* Memory::ResolveToAddress(int changelist_number, const string& stateless) {
  // Supports numbers and `names`
  // No operator support yet
  if(stateless[0] == '`')
    return get_address_by_name(stateless.substr(1, stateless.size()-2));
  else
    return get_address_by_location(stoi(stateless));
}

//This is the commit function
//Add History functionality, and do it soon
void Memory::Commit(Changelist* c) {
  ChangelistIterator it = c->get_first_change();
  do {
    //INFO << "committing: " << it->second << endl;
    // OMG, History code
    it->first->set8(c->get_changelist_number(), it->second);
  } while(c->get_next_change(&it));
}

}
