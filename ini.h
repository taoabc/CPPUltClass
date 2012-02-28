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

	Ini(const std::wstring& filename);
	void AssignFile(const std::wstring& filename);
	bool WriteString(const std::wstring& section, const std::wstring& entry, const std::wstring& str);
	std::wstring GetString(const std::wstring& section, const std::wstring& entry, const wchar_t* string_default=NULL);
	bool WriteInt(const std::wstring& section, const std::wstring& entry, int number);
	int GetInt(const std::wstring& section, const std::wstring& entry, int num_default = -1);
private:
	enum {
    kMaxValueLen = 1024,
    kMaxIntLength = 256
  };
	std::wstring config_filename_;
};
} //namespace ult

#endif // ULT_INI_H_