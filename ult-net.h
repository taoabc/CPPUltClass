/*
** Õ¯¬ÁÕ®–≈¿‡
** author
**   taoabc@gmail.com
*/
#pragma once

namespace ult {

bool InitSocket(void);
bool ParseHost(const char* raw_host, char* ip_host);
void Cleanup(void);

class Udp
{
public:
  Udp(void);
  ~Udp(void);

  bool SendTo(const char* data, int len, const char* host, u_short port);
  int RecvFrom(char* data, int buf_len);
  int RecvFrom(char* data, int buf_len, u_short port);
  void Close(void);
private:
  SOCKET socket_;
};
}