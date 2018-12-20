#include "CIniFile.h"
#include <windows.h>

CIniFile::CIniFile()
{
}


CIniFile::CIniFile(LPCTSTR szFile)
{
	m_szFileName = szFile;
}

CIniFile::~CIniFile()
{
	delete[] m_szFileName;
}

/** ��ȡ�����ļ� */
int CIniFile::GetValueInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName)
{
	return ::GetPrivateProfileInt(lpszSectionName, lpszKeyName, -1, m_szFileName);
}
