#ifndef _WINDOWS_
#include <Windows.h>
#endif

int ShellExecAsUser(const TCHAR *pcOperation, const TCHAR *pcFileName, const TCHAR *pcParameters, const HWND hwnd);
