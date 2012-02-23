#pragma once

#include <atlstr.h>

class UltIni
{
public:
	UltIni(void);
	~UltIni(void);

	UltIni(const CString& fileName);
	void AssignFile(const CString& fileNname);
	bool WriteString(const wchar_t* section, const wchar_t* entry, const wchar_t* str);
	CString GetString(const wchar_t* section, const wchar_t* entry);
	bool WriteInt(const wchar_t* section, const wchar_t* entry, int number);
	int GetInt(const wchar_t* section, const wchar_t* entry, int number = -1);
private:
	enum {MAX_VALUE_LEN = 1024};
	CString configFile_;
};