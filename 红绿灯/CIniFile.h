#pragma once
#include <Windows.h>

#ifndef __CINIFILE_H__
#define __CINIFILE_H__

class CIniFile
{
public:
	CIniFile();
	CIniFile(LPCTSTR szFile);
	virtual ~CIniFile();

	int GetValueInt(LPCTSTR lpszSection, LPCTSTR lpszKey);


private:
	LPCTSTR m_szFileName; // .\\Config.ini
};

#endif
