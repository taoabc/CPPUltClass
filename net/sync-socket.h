/*
** 同步socket的简单封装
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_SYNCSOCKET_H_
#define ULT_NET_SYNCSOCKET_H_

#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

namespace ult {

inline bool SocketStartup(void) {
  WORD version_requested;
  WSADATA wsadata;
  int err;
  version_requested = MAKEWORD(2, 2);
  err = WSAStartup(version_requested, &wsadata);
  if (0 != err) {
    return false;
  }
  if (2 != LOBYTE(wsadata.wVersion) ||
    2 != HIBYTE(wsadata.wVersion)) {
      WSACleanup();
      return false;
    }
    return true;
}

inline void SocketCleanup(void) {
  WSACleanup();
}

inline unsigned long HostToInetAddr(const char* raw_host) {
  hostent* phost;
  unsigned long result = inet_addr(raw_host);
  if (result == INADDR_NONE) {
    phost = gethostbyname(raw_host);
    if (phost != NULL) {
      in_addr inaddr;
      memcpy(&(inaddr.s_addr), phost->h_addr_list[0], phost->h_length);
      result = inaddr.s_addr;
    }
  }
  return result;
}

class Tcp {

public:

  Tcp(void) :
      socket_(NULL) {
  }

  ~Tcp(void) {
    Close();
  }

  int Connect(const char* host, u_short port, unsigned millisec = 0) {
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = HostToInetAddr(host);
    addr.sin_port = htons(port);
    if (millisec == 0) {
      return connect(socket_, (sockaddr*)&addr, sizeof (addr));
    }
    //if set timeout
    fd_set set;
    u_long ul = 1;
    ioctlsocket(socket_, FIONBIO, &ul);
    int result = SOCKET_ERROR;
    if (SOCKET_ERROR == connect(socket_, (sockaddr*)&addr, sizeof (addr))) {
      timeval tm;
      tm.tv_sec = millisec / 1000;
      tm.tv_usec = (millisec % 1000) * 1000;
      FD_ZERO(&set);
      FD_SET(socket_, &set);
      if (0 < select(socket_+1, NULL, &set, NULL, &tm)) {
        int error = SOCKET_ERROR;
        int len = sizeof (error);
        getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
        if (error == 0) {
          result = 0;
        }
      }
    }
    ul = 0;
    ioctlsocket(socket_, FIONBIO, &ul);
    return result;
  }

  int Send(const char* data, int len, unsigned millisec = 0) {
    if (millisec > 0) {
      setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (char*)&millisec, sizeof (millisec));
    }
    int sended = 0;
    int ret;
    while (sended < len) {
      ret = send(socket_, data+sended, len-sended, 0);
      if (ret == SOCKET_ERROR) {
        break;
      }
      sended += ret;
    }
    return sended;
  }

  int Recv(char* buffer, int len, unsigned millisec = 0) {
    if (millisec > 0) {
      setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char*)&millisec, sizeof (millisec));
    }
    return recv(socket_, buffer, len, 0);
  }

  int Close(void) {
    int result = 0;
    if (socket_ != NULL) {
      result = closesocket(socket_);
    }
    socket_ = NULL;
    return result;
  }

private:

  SOCKET socket_;

}; //class Tcp

class Udp {

public:

  Udp(void) :
      socket_(NULL) {
  }

  ~Udp(void) {
    Close();
  }

  int SendTo(const char* data, int len, const char* host, u_short port, unsigned millisec = 0) {
    Close();
    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = HostToInetAddr(host);
    addr.sin_port = htons(port);
    if (millisec > 0) {
      setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (char*)&millisec, sizeof (millisec));
    }
    int sended = 0;
    int ret;
    while (sended < len) {
      ret = sendto(socket_, data+sended, len-sended, 0, (sockaddr*)&addr, sizeof(addr));
      if (ret == SOCKET_ERROR) {
        break;
      }
      sended += ret;
    }
    return sended;
  }

  int RecvFrom(char* buffer, int len, unsigned millisec = 0) {
    sockaddr_in send_addr;
    int send_addr_size = sizeof (send_addr);
    if (millisec > 0) {
      setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char*)&millisec, sizeof (millisec));
    }
    return recvfrom(socket_, buffer, len, 0, (sockaddr*)&send_addr, &send_addr_size);
  }

  int RecvFrom(char* buffer, int len, u_short port, unsigned millisec = 0) {
    SOCKET recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    sockaddr_in recv_addr;
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(port);
    bind(recv_socket, (sockaddr*)&recv_socket, sizeof (recv_addr));
    sockaddr_in send_addr;
    int send_addr_size = sizeof (send_addr);
    if (millisec > 0) {
      setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char*)&millisec, sizeof (millisec));
    }
    int recv_len = recvfrom(recv_socket, buffer, len, 0, (sockaddr*)&send_addr, &send_addr_size);
    closesocket(recv_socket);
    return recv_len;
  }

  int Close(void) {
    int result = 0;
    if (socket_ != NULL) {
      result = closesocket(socket_);
    }
    socket_ = NULL;
    return result;
  }

private:

  SOCKET socket_;

}; //class Udp

} //namespace ult

#endif