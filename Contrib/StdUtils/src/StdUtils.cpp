///////////////////////////////////////////////////////////////////////////////
// StdUtils plug-in for NSIS
// Copyright (C) 2004-2015 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// http://www.gnu.org/licenses/lgpl-2.1.txt
///////////////////////////////////////////////////////////////////////////////

#include "StdUtils.h"
#include "ShellExecAsUser.h"
#include "ParameterParser.h"
#include "InvokeShellVerb.h"
#include "UnicodeSupport.h"
#include "DetectOsVersion.h"
#include "WinUtils.h"
#include "FileUtils.h"
#include "HashUtils.h"

bool g_bStdUtilsVerbose = false;

static HANDLE g_hInstance;
static RTL_CRITICAL_SECTION g_mutex;
static bool g_bCallbackRegistred;

///////////////////////////////////////////////////////////////////////////////
// DLL MAIN
///////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		InitializeCriticalSection(&g_mutex);
		g_hInstance = hInst;
		g_bCallbackRegistred = false;
		g_bStdUtilsVerbose = false;
	}
	else if(ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		DeleteCriticalSection(&g_mutex);
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
// TIME UTILS
///////////////////////////////////////////////////////////////////////////////

static const unsigned __int64 FTIME_SECOND = 10000000ui64;
static const unsigned __int64 FTIME_MINUTE = 60ui64 * FTIME_SECOND;
static const unsigned __int64 FTIME_HOUR   = 60ui64 * FTIME_MINUTE;
static const unsigned __int64 FTIME_DAY    = 24ui64 * FTIME_HOUR;

static unsigned __int64 getFileTime(void)
{
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	
	FILETIME filetime;
	if(!SystemTimeToFileTime(&systime, &filetime))
	{
		return 0;
	}

	ULARGE_INTEGER uli;
	uli.LowPart = filetime.dwLowDateTime;
	uli.HighPart = filetime.dwHighDateTime;

	return uli.QuadPart;
}

NSISFUNC(Time)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	long t = time(NULL);
	pushint(t);
}

NSISFUNC(GetMinutes)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	unsigned __int64 ftime = getFileTime() / FTIME_MINUTE;
	pushint(static_cast<int>(ftime));
}

NSISFUNC(GetHours)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	unsigned __int64 ftime = getFileTime() / FTIME_HOUR;
	pushint(static_cast<int>(ftime));
}

NSISFUNC(GetDays)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	unsigned __int64 ftime = getFileTime() / FTIME_DAY;
	pushint(static_cast<int>(ftime));
}

///////////////////////////////////////////////////////////////////////////////
// PRNG FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

#include "RandUtils.h"

NSISFUNC(Rand)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	unsigned int r = next_rand() % static_cast<unsigned int>(INT_MAX);
	pushint(r);
}

NSISFUNC(RandMax)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	int m = abs(popint()) + 1;
	unsigned int r = next_rand() % static_cast<unsigned int>(m);
	pushint(r);
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
	unsigned int r = next_rand() % static_cast<unsigned int>(diff);
	pushint(r + min);
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
		if(g_bStdUtilsVerbose)
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
		unsigned int rnd = next_rand() % static_cast<unsigned int>(max);
		if(!list[rnd])
		{
			list[rnd] = true;
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
// STRING FUNCTIONS
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

NSISFUNC(ValidFileName)
{
	static const TCHAR *const RESERVED = T("<>:\"/\\|?*");

	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(str, g_stringsize);

	popstringn(str, 0);

	bool flag = true;
	TCHAR last = 0x0;

	if(!str[0])
	{
		flag = false;
		goto exit209;
	}

	for(size_t i = 0; str[i]; i++)
	{
		if(ISCNTRL(str[i]))
		{
			flag = false;
			goto exit209;
		}
		for(size_t j = 0; RESERVED[j]; j++)
		{
			if(str[i] == RESERVED[j])
			{
				flag = false;
				goto exit209;
			}
		}
		last = str[i];
	}

	if((last == T(' ')) || (last == T('.')))
	{
		flag = false;
	}

exit209:
	pushstring(flag ? T("ok") : T("invalid"));
	delete [] str;
}

NSISFUNC(ValidPathSpec)
{
	static const TCHAR *const RESERVED = T("<>\"|?*");

	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(str, g_stringsize);

	popstringn(str, 0);

	bool flag = true;
	TCHAR last = 0x0;

	if(!str[0])
	{
		flag = false;
		goto exit209;
	}

	for(size_t i = 0; str[i]; i++)
	{
		if(ISCNTRL(str[i]))
		{
			flag = false;
			goto exit209;
		}
		for(size_t j = 0; RESERVED[j]; j++)
		{
			if(str[i] == RESERVED[j])
			{
				flag = false;
				goto exit209;
			}
		}
		if(((i == 0) && (!ISALPHA(str[i]))) || ((i == 1) && (str[i] != T(':'))) || ((i != 1) && (str[i] == T(':'))) || ((i == 2) && (str[i] != T('/')) && (str[i] != T('\\'))))
		{
			flag = false;
			goto exit209;
		}
		last = str[i];
	}

	if((last == T(' ')) || (last == T('.')))
	{
		flag = false;
	}

exit209:
	pushstring(flag ? T("ok") : T("invalid"));
	delete [] str;
}

///////////////////////////////////////////////////////////////////////////////
// SHELL FILE FUNCTIONS
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

	if((result != 0) && g_bStdUtilsVerbose)
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

	if((result != 0) && g_bStdUtilsVerbose)
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
// APPEND TO FILE
///////////////////////////////////////////////////////////////////////////////

NSISFUNC(AppendToFile)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(from, g_stringsize);
	MAKESTR(dest, g_stringsize);

	const DWORD maxlen = static_cast<DWORD>(MAX_VAL(0, popint()));
	const DWORD offset = static_cast<DWORD>(MAX_VAL(0, popint()));
	popstringn(dest, 0);
	popstringn(from, 0);
	
	unsigned long long bytesCopied = 0;
	if(AppendToFile(from, dest, offset, maxlen, &bytesCopied))
	{
		pushint(static_cast<int>(MIN_VAL(bytesCopied, static_cast<unsigned long long>(INT_MAX))));
	}
	else
	{
		pushstring(T("error"));
	}

	delete [] from;
	delete [] dest;
}

///////////////////////////////////////////////////////////////////////////////
// EXEC SHELL AS USER
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
	
	if(_tcslen(file) < 1) { delete [] file; file = NULL; }
	if(_tcslen(verb) < 1) { delete [] verb; verb = NULL; }
	if(_tcslen(args) < 1) { delete [] args; args = NULL; }

	if(!(file))
	{
		pushstring(T("einval"));
		if(file) delete [] file;
		if(verb) delete [] verb;
		if(args) delete [] args;
		return;
	}

	int result = ShellExecAsUser(verb, file, args, hWndParent, true);
	
	switch(result)
	{
	case SHELLEXECASUSER_ERROR_SUCCESS:
		pushstring(T("ok"));
		break;
	case SHELLEXECASUSER_ERROR_FALLBACK:
		pushstring(T("fallback"));
		break;
	case SHELLEXECASUSER_ERROR_FAILED:
		pushstring(T("error"));
		break;
	case SHELLEXECASUSER_ERROR_TIMEOUT:
		pushstring(T("timeout"));
		break;
	case SHELLEXECASUSER_ERROR_NOTFOUND:
		pushstring(T("not_found"));
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
// INVOKE SHELL VERB
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
		pushstring(T("timeout"));
		break;
	case -3:
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
// EXEC SHELL WAIT
///////////////////////////////////////////////////////////////////////////////

NSISFUNC(ExecShellWaitEx)
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

	if(ShellExecuteEx(&shInfo) != FALSE)
	{
		if(VALID_HANDLE(shInfo.hProcess))
		{
			TCHAR out[32];
			SNPRINTF(out, 32, T("hProc:%08X"), shInfo.hProcess);
			pushstring(out);
			pushstring(_T("ok"));
		}
		else
		{
			pushint(0);
			pushstring(T("no_wait"));
		}
	}
	else
	{
		pushint(GetLastError());
		pushstring(T("error"));
	}

	delete [] file;
	delete [] verb;
	delete [] args;
}

NSISFUNC(WaitForProcEx)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(temp, g_stringsize);
	popstringn(temp, 0);

	HANDLE hProc = NULL;
	int result = SSCANF(temp, T("hProc:%X"), &hProc);

	DWORD dwExitCode = 0;
	bool success = false;

	if(result == 1)
	{
		if(hProc != NULL)
		{
			if(WaitForSingleObject(hProc, INFINITE) == WAIT_OBJECT_0)
			{
				success = (GetExitCodeProcess(hProc, &dwExitCode) != FALSE);
			}
			CloseHandle(hProc);
		}
	}

	if(success)
	{
		pushint(dwExitCode);
	}
	else
	{
		pushstring(T("error"));
	}

	delete [] temp;
}


///////////////////////////////////////////////////////////////////////////////
// GET COMMAND-LINE PARAMS
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
// GET REAL OS VERSION
///////////////////////////////////////////////////////////////////////////////

NSISFUNC(GetRealOsVersion)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	bool flag;
	unsigned int version[3];

	if(get_real_os_version(&version[0], &version[1], &version[2], &flag))
	{
		pushint(version[2]);
		pushint(version[1]);
		pushint(version[0]);
	}
	else
	{
		pushstring(T("error"));
		pushstring(T("error"));
		pushstring(T("error"));
	}
}

NSISFUNC(GetRealOsBuildNo)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	bool flag;
	unsigned int buildNumber;

	if(get_real_os_buildNo(&buildNumber, &flag))
	{
		pushint(buildNumber);
	}
	else
	{
		pushstring(T("error"));
	}
}

NSISFUNC(VerifyRealOsVersion)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	bool flag;
	unsigned int expectedVersion[3];
	unsigned int detectedVersion[3];

	expectedVersion[2] = abs(popint());
	expectedVersion[1] = abs(popint());
	expectedVersion[0] = abs(popint());

	if(!get_real_os_version(&detectedVersion[0], &detectedVersion[1], &detectedVersion[2], &flag))
	{
		pushstring(T("error"));
		return;
	}

	//Majaor version
	for(size_t i = 0; i < 3; i++)
	{
		if(detectedVersion[i] > expectedVersion[i])
		{
			pushstring(T("newer"));
			return;
		}
		if(detectedVersion[i] < expectedVersion[i])
		{
			pushstring(T("older"));
			return;
		}
	}

	pushstring(T("ok"));
}

NSISFUNC(VerifyRealOsBuildNo)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	bool flag;
	unsigned int expectedBuildNo;
	unsigned int detectedBuildNo;

	expectedBuildNo = abs(popint());
	
	if(!get_real_os_buildNo(&detectedBuildNo, &flag))
	{
		pushstring(T("error"));
		return;
	}

	if(detectedBuildNo > expectedBuildNo)
	{
		pushstring(T("newer"));
		return;
	}
	if(detectedBuildNo < expectedBuildNo)
	{
		pushstring(T("older"));
		return;
	}

	pushstring(T("ok"));
}

NSISFUNC(GetRealOsName)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	bool flag;
	unsigned int detectedVersion[3];

	if(!get_real_os_version(&detectedVersion[0], &detectedVersion[1], &detectedVersion[2], &flag))
	{
		pushstring(T("error"));
		return;
	}

	pushstring(get_os_friendly_name(detectedVersion[0], detectedVersion[1]));
}

NSISFUNC(GetOsEdition)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	bool isServerEdition;
	if(!get_os_server_edition(isServerEdition))
	{
		pushstring(T("error"));
		return;
	}

	pushstring(isServerEdition ? T("server") : T("workstation"));
}

///////////////////////////////////////////////////////////////////////////////
// HASH COMPUTATION
///////////////////////////////////////////////////////////////////////////////

static int GetHashType(const TCHAR *const type)
{
	static const struct
	{
		const TCHAR *const name;
		const int id;
	}
	HASH_ALGO_MAPPING[] =
	{
		{ T("CRC-32"),     STD_HASHTYPE_CRC_32   },
		{ T("MD5-128"),    STD_HASHTYPE_MD5_128  },
		{ T("SHA1-160"),   STD_HASHTYPE_SHA1_160 },
		{ T("SHA2-224"),   STD_HASHTYPE_SHA2_224 },
		{ T("SHA2-256"),   STD_HASHTYPE_SHA2_256 },
		{ T("SHA2-384"),   STD_HASHTYPE_SHA2_384 },
		{ T("SHA2-512"),   STD_HASHTYPE_SHA2_512 },
		{ T("SHA3-224"),   STD_HASHTYPE_SHA3_224 },
		{ T("SHA3-256"),   STD_HASHTYPE_SHA3_256 },
		{ T("SHA3-384"),   STD_HASHTYPE_SHA3_384 },
		{ T("SHA3-512"),   STD_HASHTYPE_SHA3_512 },
		{ T("BLAKE2-224"), STD_HASHTYPE_BLK2_224 },
		{ T("BLAKE2-256"), STD_HASHTYPE_BLK2_256 },
		{ T("BLAKE2-384"), STD_HASHTYPE_BLK2_384 },
		{ T("BLAKE2-512"), STD_HASHTYPE_BLK2_512 },
		{ NULL, -1 }
	};

	for(size_t i = 0; HASH_ALGO_MAPPING[i].name; i++)
	{
		if(STRICMP(type, HASH_ALGO_MAPPING[i].name) == 0)
		{
			return HASH_ALGO_MAPPING[i].id;
		}
	}
	return -1;
}

NSISFUNC(HashFile)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(file, g_stringsize);
	MAKESTR(temp, g_stringsize);

	popstringn(file, 0);
	popstringn(temp, 0);

	const int hashType = GetHashType(STRTRIM(temp));
	if(hashType >= 0)
	{
		if(ComputeHash_FromFile(hashType, file, temp, g_stringsize))
		{
			pushstring(temp);
		}
		else
		{
			pushstring(T("error"));
		}
	}
	else
	{
		pushstring(T("invalid"));
	}

	delete [] file;
	delete [] temp;
}

NSISFUNC(HashText)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	MAKESTR(temp, g_stringsize);
	MAKESTR(text, g_stringsize);

	popstringn(text, 0);
	popstringn(temp, 0);

	const int hashType = GetHashType(STRTRIM(temp));
	if(hashType >= 0)
	{
		if(ComputeHash_FromText(hashType, text, temp, g_stringsize))
		{
			pushstring(temp);
		}
		else
		{
			pushstring(T("error"));
		}
	}
	else
	{
		pushstring(T("invalid"));
	}

	delete [] text;
	delete [] temp;
}

///////////////////////////////////////////////////////////////////////////////
// FOR DEBUGGING
///////////////////////////////////////////////////////////////////////////////

NSISFUNC(EnableVerboseMode)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	g_bStdUtilsVerbose = true;
}

NSISFUNC(DisableVerboseMode)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	g_bStdUtilsVerbose = false;
}

///////////////////////////////////////////////////////////////////////////////

#include "resource.h"

static const TCHAR *dllTimeStamp = T(__DATE__) T(", ") T(__TIME__);
static const TCHAR *dllVerString = T(DLL_VERSION_STRING);

NSISFUNC(GetLibVersion)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
	pushstring(dllTimeStamp);
	pushstring(dllVerString);
}

///////////////////////////////////////////////////////////////////////////////

NSISFUNC(Dummy)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);
}
