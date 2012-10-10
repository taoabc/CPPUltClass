/*
** httpÏÂÔØÆ÷µÄ×´Ì¬´úÂë
** author
**   taoabc@gmail.com
*/

#ifndef ULT_NET_WINHTTPDOWNLOADERSTATUS_H_
#define ULT_NET_WINHTTPDOWNLOADERSTATUS_H_

namespace ult{

namespace HttpStatus {
enum {
  kContentUncomplete   = -7,
  kWriteFileError      = -6,
  kCreateFileError     = -5,
  kDownloadError       = -4,
  kSendRequestError    = -3,
  kConnectError        = -2,
  kUnknownError        = -1,
  kSuccess             = 0,
  kConnecting          = 1,
  kReceiveing          = 2,
  kDownloading         = 3,
};
}//namespace HttpStatus
}//namespace ult

#endif