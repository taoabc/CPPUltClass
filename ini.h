/*
** ini文件操作类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_INI_H_
#define ULT_INI_H_

#include <string>

namespace ult {
class Ini
{
public:
	Ini(void);
  ~Ini(void);

	Ini(const wchar_t* filename);
	void AssignFile(const wchar_t* filename);
	bool WriteString(const wchar_t* section, const wchar_t* entry, const wchar_t* str);
	std::wstring GetString(const wchar_t* section, const wchar_t* entry, const wchar_t* string_default=NULL);
	bool WriteInt(const wchar_t* section, const wchar_t* entry, int number);
	int GetInt(const wchar_t* section, const wchar_t* entry, int num_default = -1);
private:
	enum {
    kMaxValueLen = 1024,
    kMaxIntLength = 256
  };
	std::wstring config_filename_;
};
} //namespace ult

#endif // ULT_INI_H_