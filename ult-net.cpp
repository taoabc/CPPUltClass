#include "StdAfx.h"
#include ".\ult-net.h"

namespace ult {
bool g_is_socket_init = false;

bool InitSocket( void )
{
  if (g_is_socket_init) {
    return true;
  }
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
    g_is_socket_init = true;
    return true;
}

bool ParseHost( const char* raw_host, char* ip_host )
{
  struct hostent *phost;
  if (INADDR_NONE != inet_addr(raw_host)) {
    strcpy(ip_host, raw_host);
  } else {
    phost = gethostbyname(raw_host);
    if (NULL == phost) {
      return false;
    }
    struct in_addr inaddr;
    memcpy(&inaddr.s_addr, phost->h_addr_list[0], phost->h_length);
    strcpy(ip_host, inet_ntoa(inaddr));
  }
  return true;
}

void Cleanup( void )
{
  WSACleanup();
}


Udp::Udp( void ) :
  socket_(NULL)
{
  InitSocket();
}

Udp::~Udp( void )
{

}

void Udp::Close( void )
{
  closesocket(socket_);
}

void Udp::SetRecvTimeout(int millisec)
{
  if (NULL != socket_) {
    int ret = setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char*)&millisec, sizeof (millisec));
  }
}

bool Udp::SendTo( const char* data, int len, const char* host, u_short port )
{
  char ip_host[64];
  if (!ParseHost(host, ip_host)) {
    return false;
  }
  socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip_host);
  addr.sin_port = htons(port);
  int ret = sendto(socket_, data, len, 0, (SOCKADDR*)&addr, sizeof(addr));
  if (SOCKET_ERROR == ret || ret != len) {
    return false;
  } else {
    return true;
  }
}

int Udp::RecvFrom( char* data, int buf_len )
{
  sockaddr_in send_addr;
  int send_addr_size = sizeof (send_addr);
  return recvfrom(socket_, data, buf_len, 0, (SOCKADDR*)&send_addr, &send_addr_size);
}

int Udp::RecvFrom( char* data, int buf_len, u_short port )
{
  SOCKET recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  sockaddr_in recv_addr;
  recv_addr.sin_family = AF_INET;
  recv_addr.sin_port = htons(port);
  recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bind(recv_socket, (SOCKADDR*)&recv_addr, sizeof(recv_addr));
  sockaddr_in send_addr;
  int send_addr_size = sizeof (send_addr);
  int recv_len = recvfrom(recv_socket, data, buf_len, 0, (SOCKADDR*)&send_addr, &send_addr_size);
  closesocket(recv_socket);
  return recv_len;
}

}