#include "LoadDll.h"

//------------------------------------------------------------------------
// ����������
//------------------------------------------------------------------------
TLoadDll::TLoadDll(PCHAR DllName)
{
	// ������������� ������ DLL
	hDll = ::LoadLibrary(DllName);
}

//------------------------------------------------------------------------
// ����������
//------------------------------------------------------------------------
TLoadDll::~TLoadDll()
{
	::FreeLibrary(hDll);
}

//------------------------------------------------------------------------
// ��������� ������ ������� ������������ ��������� �� ��������� ������
//------------------------------------------------------------------------
LPVOID WINAPI TLoadDll::CallCreateLInstance(void)
{
	if(hDll == NULL) return NULL;
	return FARPROC(::GetProcAddress(hDll, "CreateLInstance"));
}

//------------------------------------------------------------------------
// ��������� ������ ������� ������������ ������ DLL
//------------------------------------------------------------------------
LPVOID WINAPI TLoadDll::CallGetDllVersion(void)
{
	if(hDll == NULL) return NULL;
	return FARPROC(::GetProcAddress(hDll, "GetDllVersion"));
}

//------------------------------------------------------------------------
// ��������� �������������� ����������
//------------------------------------------------------------------------
HINSTANCE WINAPI TLoadDll::GetDllHinstance(void)
{
	return hDll;
}
