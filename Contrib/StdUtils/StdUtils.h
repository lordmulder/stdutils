#ifndef __STDUTILS_H__
#define __STDUTILS_H__

#include <Windows.h>
#include "nsis\pluginapi.h"
#include "msvc_utils.h"

#ifndef UNICODE
#define _T(X) X
#endif

#define NSISFUNC(name) extern "C" void __declspec(dllexport) name(HWND hWndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters* extra)

#endif //__STDUTILS_H__
