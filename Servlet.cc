// Servlet.cc -- May 24, 2009
//    by geohot
// part of "The Embedded Disassembler"
//  released under GPLv3, see http://gplv3.fsf.org/

#include "Servlet.h"
#include "util.h"


using namespace std;
using namespace eda;

// This is where the cool shit is at
template <class T> bool Servlet<T>::ProcessRequest(const string& in, string* out) {
  string command = in.substr(0, in.find_first_of(' '));
  string address = in.substr(command.length() + 1, in.find_first_of(' ', command.length() + 1)-command.length()-1);

  int t;
  while ((t = address.find_first_of('%')) != string::npos) {
    string pchar = address.substr(t+1, 2);
    address.replace(t, 3, 1, (char)stoi("0x"+pchar));
  }


  CommandTableIterator it = command_table_.find(command);

  LOG(INFO) << command << " " << address;

  if (it == command_table_.end()) {
    LOG(WARNING) << "Command not found";
    return false;
  }
  vector<string> address_parsed;
  StringSplit("/", address, &address_parsed);

  LOG(INFO) << "Request size: " << address_parsed.size();

  return (it->second.first->*(it->second.second))(address_parsed, out);
}

template <class T> void Servlet<T>::RegisterCommandHandler(const std::string& command, T* mem, callback handle) {
  command_table_.insert(make_pair(command, make_pair(mem, handle)));
}

// Boring server shit below this line
template <class T> bool Servlet<T>::StartServer(int port) {
  // Init WinSock
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup(0x0101, &wsaData);
#endif
  // Open
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ == 0) {
    LOG(ERROR) << "socket open failed";
    return false;
  }

  socket_address_.sin_family = AF_INET;
  socket_address_.sin_addr.s_addr = htonl(INADDR_ANY);
  socket_address_.sin_port = htons(port);

  if (bind(socket_, (struct sockaddr*)&socket_address_, sizeof(socket_address_)) < 0) {
    LOG(ERROR) << "bind failed";
    return false;
  }

  if (listen(socket_, SOMAXCONN) == -1) {
    LOG(ERROR) << "listen failed";
    return false;
  }

  LOG(INFO) << "Server started";

  return RunLoop();
}

template <class T> bool Servlet<T>::EndServer() {
  LOG(INFO) << "Ending server";
  shutdown(socket_, SHUT_RDWR);
  return true;
}

// I have to fix this someday
#define RECV_CHUNK 0x1000

template <class T> bool Servlet<T>::RunLoop() {
  while(AcceptClient()) {
    LOG(INFO) << "Got client";
    // Read all into string
    char temp_data[RECV_CHUNK];

    string data;
    int size = recv(client_socket_, temp_data, RECV_CHUNK, 0);
    data.append(temp_data, size);

    string out;
    if(ProcessRequest(data, &out)) {
      out = "HTTP/1.1 200\n\n" + out;
      send(client_socket_, out.data(), out.size(), 0);
      LOG(INFO) << "Reply sent(" << out.size() << ")";
    }
    else
      LOG(WARNING) << "Bad request";

    shutdown(client_socket_, SHUT_RDWR);
  }
  return true;
}

template <class T> bool Servlet<T>::AcceptClient() {
  int client_socket;
  struct sockaddr_in client_address;
  socklen_t sin_size=sizeof(client_address);
  if ((client_socket = accept(socket_, (struct sockaddr *)&client_address, &sin_size)) == -1) {
    //LOG(WARNING) << "accept failed" << std::endl;
    return false;
  }
  //client_sockets_.push_back(client_socket);
  client_socket_ = client_socket;
  return true;
}

template class Servlet<FactoryOwner>;
