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

#ifdef WIN32            //Windows sockets
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#define SHUT_RDWR SD_SEND
#define socklen_t int

#else                   //Linux sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#endif

namespace eda {

class FactoryOwner;

template<class T>
class Servlet {
public:
  typedef bool (T::*callback)(const std::vector<std::string>& argv, std::string* out);
  void RegisterCommandHandler(const std::string& command, T* mem, callback handle);
  bool StartServer(int port);   // this is the main server run loop
  bool EndServer();
private:
  bool RunLoop();
  bool AcceptClient();          // To be called by RunLoop
  bool ProcessRequest(const std::string& in, std::string* out);
  std::map<std::string, std::pair<T*, callback> > command_table_;
  typedef typename std::map<std::string, std::pair<T*, callback> >::iterator CommandTableIterator;
  //std::vector<int> client_sockets_;
  int client_socket_;
  struct sockaddr_in socket_address_;
  int socket_;
};

};

