///////////////////////////////////////////////////////////////////////////////
// StdUtils plug-in for NSIS
// Copyright (C) 2004-2012 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// http://www.gnu.org/licenses/gpl-2.0.txt
///////////////////////////////////////////////////////////////////////////////

#include "StdUtils.h"
#include "ShellExecAsUser.h"
#include "ParameterParser.h"
#include "InvokeShellVerb.h"
#include "UnicodeSupport.h"

HANDLE g_hInstance;
bool g_bCallbackRegistred;
bool g_bVerbose;

///////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInst;
		g_bCallbackRegistred = false;
		g_bVerbose = false;
		srand(static_cast<unsigned int>(time(NULL)));
	}
	return TRUE;
}

static UINT_PTR PluginCallback(enum NSPIM msg)
{
	switch(msg)
	{
	case NSPIM_UNLOAD:
	case NSPIM_GUIUNLOAD:
		break;
	default:
		MessageBoxA(NULL, "Unknown callback message. Take care!", "StdUtils", MB_ICONWARNING|MB_TOPMOST|MB_TASKMODAL);
		break;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(Time)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	long t = time(NULL);
	pushint(t);
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(Rand)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	int r = (rand() * RAND_MAX) + rand();
	pushint(r);
}

NSISFUNC(RandMax)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	int m = abs(popint()) + 1;
	int r = (m > RAND_MAX) ? ((rand() * RAND_MAX) + rand()) : rand();
	pushint(r % m);
}

NSISFUNC(RandMinMax)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	int max = popint();
	int min = popint();
	
	if(min > max)
	{
		MessageBoxW(NULL, L"RandMinMax() was called with bad arguments!", L"StdUtils::RandMinMax", MB_ICONERROR | MB_TASKMODAL);
		pushint(0);
	}

	int diff = abs(max - min) + 1;
	int r = (diff > RAND_MAX) ? ((rand() * RAND_MAX) + rand()) : rand();
	pushint((r % diff) + min);
}

NSISFUNC(RandList)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	int count = popint();
	int max = popint() + 1;
	int done = 0;

	if(count > max)
	{
		if(g_bVerbose)
		{
			MessageBoxW(NULL, L"RandList() was called with bad arguments!", L"StdUtils::RandList", MB_ICONERROR | MB_TASKMODAL);
		}
		pushstring(T("EOL"));
		return;
	}

	bool *list = new bool[max];
	for(int idx = 0; idx < max; idx++)
	{
		list[idx] = false;
	}

	while(done < count)
	{
		int rnd = (max > RAND_MAX) ? ((rand() * RAND_MAX) + rand()) : rand();
		int idx = rnd % max;
		if(!list[idx])
		{
			list[idx] = true;
			done++;
		}
	}

	pushstring(T("EOL"));
	for(int idx = max-1; idx >= 0; idx--)
	{
		if(list[idx])
		{
			pushint(idx);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(FormatStr)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(fmt, g_stringsize);
	MAKESTR(out, g_stringsize);

	int v = popint();
	popstringn(fmt, 0);

	if(SNPRINTF(out, g_stringsize, fmt, v) < 0)
	{
		out[g_stringsize-1] = T('\0');
	}

	pushstring(out);
	delete [] fmt;
	delete [] out;
}

NSISFUNC(FormatStr2)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(fmt, g_stringsize);
	MAKESTR(out, g_stringsize);

	int v2 = popint();
	int v1 = popint();
	popstringn(fmt, 0);

	if(SNPRINTF(out, g_stringsize, fmt, v1, v2) < 0)
	{
		out[g_stringsize-1] = T('\0');
	}

	pushstring(out);
	delete [] fmt;
	delete [] out;
}

NSISFUNC(FormatStr3)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(fmt, g_stringsize);
	MAKESTR(out, g_stringsize);

	int v3 = popint();
	int v2 = popint();
	int v1 = popint();
	popstringn(fmt, 0);

	if(SNPRINTF(out, g_stringsize, fmt, v1, v2, v3) < 0)
	{
		out[g_stringsize-1] = T('\0');
	}

	pushstring(out);
	delete [] fmt;
	delete [] out;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(ScanStr)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(in, g_stringsize);
	MAKESTR(fmt, g_stringsize);

	int def = popint();
	popstringn(in, 0);
	popstringn(fmt, 0);
	int out = 0;

	if(SSCANF(in, fmt, &out) != 1)
	{
		out = def;
	}

	pushint(out);
	delete [] fmt;
	delete [] in;
}

NSISFUNC(ScanStr2)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(in, g_stringsize);
	MAKESTR(fmt, g_stringsize);

	int def2 = popint();
	int def1 = popint();
	popstringn(in, 0);
	popstringn(fmt, 0);
	int out1 = 0;
	int out2 = 0;
	int result = 0;

	result = SSCANF(in, fmt, &out1, &out2);
	
	if(result != 2)
	{
		if(result != 1)
		{
			out1 = def1;
		}
		out2 = def2;
	}

	pushint(out2);
	pushint(out1);
	delete [] fmt;
	delete [] in;
}

NSISFUNC(ScanStr3)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(in, g_stringsize);
	MAKESTR(fmt, g_stringsize);

	int def3 = popint();
	int def2 = popint();
	int def1 = popint();
	popstringn(in, 0);
	popstringn(fmt, 0);
	int out1 = 0;
	int out2 = 0;
	int out3 = 0;
	int result = 0;

	result = SSCANF(in, fmt, &out1, &out2, &out3);
	
	if(result != 3)
	{
		if(result == 0)
		{
			out1 = def1;
			out2 = def2;
		}
		else if(result == 1)
		{
			out2 = def2;
		}
		out3 = def3;
	}

	pushint(out3);
	pushint(out2);
	pushint(out1);
	delete [] fmt;
	delete [] in;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(TrimStr)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(str, g_stringsize);
	
	popstringn(str, 0);
	pushstring(STRTRIM(str));

	delete [] str;
}

NSISFUNC(TrimStrLeft)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(str, g_stringsize);
	
	popstringn(str, 0);
	pushstring(STRTRIM(str, true, false));

	delete [] str;
}

NSISFUNC(TrimStrRight)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(str, g_stringsize);
	
	popstringn(str, 0);
	pushstring(STRTRIM(str, false, true));

	delete [] str;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(RevStr)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(str, g_stringsize);
	
	popstringn(str, 0);

	if(str[0] != T('\0'))
	{
		size_t left = 0;
		size_t right = STRLEN(str) - 1;
		while(left < right)
		{
			TCHAR tmp = str[left];
			str[left++] = str[right];
			str[right--] = tmp;
		}
	}

	pushstring(str);
	delete [] str;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(SHFileMove)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(from, g_stringsize);
	MAKESTR(dest, g_stringsize);

	SHFILEOPSTRUCT fileop;
	SecureZeroMemory(&fileop, sizeof(SHFILEOPSTRUCT));

	HWND hwnd = (HWND) popint();
	popstringn(dest, 0);
	popstringn(from, 0);

	fileop.hwnd = hwnd;
	fileop.wFunc = FO_MOVE;
	fileop.pFrom = from;
	fileop.pTo = dest;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	if(hwnd == 0) fileop.fFlags |= FOF_SILENT;

	int result = SHFileOperation(&fileop);
	pushstring((result == 0) ? (fileop.fAnyOperationsAborted ? T("ABORTED") : T("OK")) : T("ERROR"));

	if((result != 0) && g_bVerbose)
	{
		char temp[1024];
		_snprintf(temp, 1024, "Failed with error code: 0x%X", result);
		temp[1023] = '\0';
		MessageBoxA(NULL, temp, "StdUtils::SHFileMove", MB_TOPMOST|MB_ICONERROR);
	}

	delete [] from;
	delete [] dest;
}

NSISFUNC(SHFileCopy)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(from, g_stringsize);
	MAKESTR(dest, g_stringsize);

	SHFILEOPSTRUCT fileop;
	SecureZeroMemory(&fileop, sizeof(SHFILEOPSTRUCT));

	HWND hwnd = (HWND) popint();
	popstringn(dest, 0);
	popstringn(from, 0);

	fileop.hwnd = hwnd;
	fileop.wFunc = FO_COPY;
	fileop.pFrom = from;
	fileop.pTo = dest;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	if(hwnd == 0) fileop.fFlags |= FOF_SILENT;

	int result = SHFileOperation(&fileop);
	pushstring((result == 0) ? (fileop.fAnyOperationsAborted ? T("ABORTED") : T("OK")) : T("ERROR"));

	if((result != 0) && g_bVerbose)
	{
		char temp[1024];
		_snprintf(temp, 1024, "Failed with error code: 0x%X", result);
		temp[1023] = '\0';
		MessageBoxA(NULL, temp, "StdUtils::SHFileCopy", MB_TOPMOST|MB_ICONERROR);
	}

	delete [] from;
	delete [] dest;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(ExecShellAsUser)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(file, g_stringsize);
	MAKESTR(verb, g_stringsize);
	MAKESTR(args, g_stringsize);

	popstringn(args, 0);
	popstringn(verb, 0);
	popstringn(file, 0);
	
	if(_tcslen(verb) < 1) { delete [] verb; verb = NULL; }
	if(_tcslen(args) < 1) { delete [] args; args = NULL; }

	int result = ShellExecAsUser(verb, file, args, hWndParent, true);
	
	switch(result)
	{
	case 1:
		pushstring(T("ok"));
		break;
	case 0:
		pushstring(T("fallback"));
		break;
	case -1:
		pushstring(T("error"));
		break;
	case -2:
		pushstring(T("timeout"));
		break;
	default:
		pushstring(T("unknown"));
		break;
	}

	if(file) delete [] file;
	if(verb) delete [] verb;
	if(args) delete [] args;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(InvokeShellVerb)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(path, g_stringsize);
	MAKESTR(file, g_stringsize);

	int verb = popint();
	popstringn(file, 0);
	popstringn(path, 0);
	
	if(_tcslen(file) < 1) { delete [] file; file = NULL; }
	if(_tcslen(path) < 1) { delete [] path; path = NULL; }

	if(!(file && path))
	{
		pushstring(T("einval"));
		if(file) delete [] file;
		if(path) delete [] path;
		return;
	}

	int result = MyInvokeShellVerb(path, file, verb, true);
	
	switch(result)
	{
	case 1:
		pushstring(T("ok"));
		break;
	case 0:
		pushstring(T("not_found"));
		break;
	case -1:
		pushstring(T("unsupported"));
		break;
	case -2:
	case -3:
	case -4:
	case -5:
	case -6:
	case -7:
	case -8:
	case -9:
	case -10:
		pushstring(T("error"));
		break;
	default:
		pushstring(T("unknown"));
		break;
	}

	if(file) delete [] file;
	if(path) delete [] path;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(ExecShellWait)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(file, g_stringsize);
	MAKESTR(verb, g_stringsize);
	MAKESTR(args, g_stringsize);
	
	popstringn(args, 0);
	popstringn(verb, 0);
	popstringn(file, 0);

	SHELLEXECUTEINFO shInfo;
	memset(&shInfo, 0, sizeof(SHELLEXECUTEINFO));
	shInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shInfo.hwnd = hWndParent;
	shInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
	shInfo.lpFile = file;
	shInfo.lpVerb = (_tcslen(verb) > 0) ? verb : NULL;
	shInfo.lpParameters = (_tcslen(args) > 0) ? args : NULL;
	shInfo.nShow = SW_SHOWNORMAL;

	if(ShellExecuteEx(&shInfo))
	{
		if((shInfo.hProcess != NULL) && (shInfo.hProcess != INVALID_HANDLE_VALUE))
		{
			TCHAR out[32];
			SNPRINTF(out, 32, T("hProc:%08X"), shInfo.hProcess);
			pushstring(out);
		}
		else
		{
			pushstring(T("no_wait"));
		}
	}
	else
	{
		pushstring(T("error"));
	}

	delete [] file;
	delete [] verb;
	delete [] args;
}

NSISFUNC(WaitForProc)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(temp, g_stringsize);
	popstringn(temp, 0);

	HANDLE hProc = NULL;
	int result = SSCANF(temp, T("hProc:%X"), &hProc);

	if(result == 1)
	{
		if(hProc != NULL)
		{
			WaitForSingleObject(hProc, INFINITE);
			CloseHandle(hProc);
		}
	}

	delete [] temp;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(GetParameter)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(aval, g_stringsize);
	MAKESTR(name, g_stringsize);

	popstringn(aval, 0);
	popstringn(name, 0);
	parse_commandline(name, aval, g_stringsize);
	pushstring(aval);

	delete [] aval;
	delete [] name;
}

NSISFUNC(GetAllParameters)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	int truncate = popint();
	const TCHAR *cmd = get_commandline_arguments();

	if((STRLEN(cmd) < g_stringsize) || truncate)
	{
		pushstring(cmd);
	}
	else
	{
		pushstring(T("too_long"));
	}
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(EnableVerboseMode)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	g_bVerbose = true;
}

NSISFUNC(DisableVerboseMode)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	g_bVerbose = false;
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(Dummy)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
}
