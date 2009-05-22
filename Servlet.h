// Servlet.h -- May 21, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/
//
// hehe, it's like Java, except Java sucks
//
// For saving, EDA doesn't waste time trying to save data structures
// It just saves the server accesses
// My laziness is awesome
// Although this is probably mad slow

#include <string>
#include <vector>
#include <map>

// Two ways to access addresses for now

// /Address/ByName/<name>/
// /Address/ByExpression/<expression>/



// /Changelist/<number>


namespace eda {

class FactoryOwner;

class Servlet {
public:
  typedef int (Memory::*callback)(const vector<string>& argv, string* out);
  void RegisterCommandHandler(const string& command, FactoryOwner* mem, callback handle);
  bool StartServer(int port);   // this is the main server run loop
private:
  map<string, pair<FactoryOwner*, callback> > command_table_;
};


};
