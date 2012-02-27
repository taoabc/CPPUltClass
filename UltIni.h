/*
** ini文件操作类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_INI_H_
#define ULT_INI_H_

#include <atlstr.h>

class UltIni
{
public:
	UltIni(void);
  ~UltIni(void);

	UltIni(const CString& filename);
	void AssignFile(const CString& filename);
	bool WriteString(const wchar_t* section, const wchar_t* entry, const wchar_t* str);
	CString GetString(const wchar_t* section, const wchar_t* entry, const wchar_t* string_default=NULL);
	bool WriteInt(const wchar_t* section, const wchar_t* entry, int number);
	int GetInt(const wchar_t* section, const wchar_t* entry, int num_default = -1);
private:
	enum {kMaxValueLen = 1024};
	CString config_filename_;
};

#endif // ULT_INI_H_