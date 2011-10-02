///////////////////////////////////////////////////////////////////////////////
// StdUtils plug-in for NSIS
// Copyright (C) 2004-2011 LoRd_MuldeR <MuldeR2@GMX.de>
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

HANDLE g_hInstance;
bool g_bVerbose;

NSISFUNC(Time)
{
	EXDLL_INIT();
	long t = time(NULL);
	pushint(t);
}

NSISFUNC(Rand)
{
	EXDLL_INIT();
	int r = (rand() * RAND_MAX) + rand();
	pushint(r);
}

NSISFUNC(RandMax)
{
	EXDLL_INIT();
	int m = abs(popint()) + 1;
	int r = (m > RAND_MAX) ? ((rand() * RAND_MAX) + rand()) : rand();
	pushint(r % m);
}

NSISFUNC(RandMinMax)
{
	EXDLL_INIT();
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
	int count = popint();
	int max = popint() + 1;
	int done = 0;

	if(count > max)
	{
		if(g_bVerbose)
		{
			MessageBoxW(NULL, L"RandList() was called with bad arguments!", L"StdUtils::RandList", MB_ICONERROR | MB_TASKMODAL);
		}
		pushstring(_T("EOL"));
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

	pushstring(_T("EOL"));
	for(int idx = max-1; idx >= 0; idx--)
	{
		if(list[idx])
		{
			pushint(idx);
		}
	}
}

NSISFUNC(FormatStr)
{
	EXDLL_INIT();
	TCHAR *fmt = new TCHAR[g_stringsize];
	TCHAR *out = new TCHAR[g_stringsize];
	SecureZeroMemory(out, sizeof(TCHAR) * g_stringsize);
	int v = popint();
	popstringn(fmt, 0);

#ifdef UNICODE
	if(_snwprintf(out, g_stringsize, fmt, v) < 0)
	{
		out[g_stringsize-1] = L'\0';
	}
#else
	if(_snprintf(out, g_stringsize, fmt, v) < 0)
	{
		out[g_stringsize-1] = '\0';
	}
#endif

	pushstring(out);
	delete [] fmt;
	delete [] out;
}

NSISFUNC(FormatStr2)
{
	EXDLL_INIT();
	TCHAR *fmt = new TCHAR[g_stringsize];
	TCHAR *out = new TCHAR[g_stringsize];
	SecureZeroMemory(out, sizeof(TCHAR) * g_stringsize);
	int v2 = popint();
	int v1 = popint();
	popstringn(fmt, 0);

#ifdef UNICODE
	if(_snwprintf(out, g_stringsize, fmt, v1, v2) < 0)
	{
		out[g_stringsize-1] = L'\0';
	}
#else
	if(_snprintf(out, g_stringsize, fmt, v1, v2) < 0)
	{
		out[g_stringsize-1] = '\0';
	}
#endif

	pushstring(out);
	delete [] fmt;
	delete [] out;
}

NSISFUNC(FormatStr3)
{
	EXDLL_INIT();
	TCHAR *fmt = new TCHAR[g_stringsize];
	TCHAR *out = new TCHAR[g_stringsize];
	SecureZeroMemory(out, sizeof(TCHAR) * g_stringsize);
	int v3 = popint();
	int v2 = popint();
	int v1 = popint();
	popstringn(fmt, 0);

#ifdef UNICODE
	if(_snwprintf(out, g_stringsize, fmt, v1, v2, v3) < 0)
	{
		out[g_stringsize-1] = L'\0';
	}
#else
	if(_snprintf(out, g_stringsize, fmt, v1, v2, v3) < 0)
	{
		out[g_stringsize-1] = '\0';
	}
#endif

	pushstring(out);
	delete [] fmt;
	delete [] out;
}

NSISFUNC(ScanStr)
{
	EXDLL_INIT();
	TCHAR *fmt = new TCHAR[g_stringsize];
	TCHAR *in = new TCHAR[g_stringsize];
	int def = popint();
	popstringn(in, 0);
	popstringn(fmt, 0);
	int out = 0;

#ifdef UNICODE
	if(swscanf(in, fmt, &out) != 1)
	{
		out = def;
	}
#else
	if(sscanf(in, fmt, &out) != 1)
	{
		out = def;
	}
#endif

	pushint(out);
	delete [] fmt;
	delete [] in;
}

NSISFUNC(ScanStr2)
{
	EXDLL_INIT();
	TCHAR *fmt = new TCHAR[g_stringsize];
	TCHAR *in = new TCHAR[g_stringsize];
	int def2 = popint();
	int def1 = popint();
	popstringn(in, 0);
	popstringn(fmt, 0);
	int out1 = 0;
	int out2 = 0;
	int result = 0;

#ifdef UNICODE
	result = swscanf(in, fmt, &out1, &out2);
#else
	result = sscanf(in, fmt, &out1, &out2);
#endif
	
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
	TCHAR *fmt = new TCHAR[g_stringsize];
	TCHAR *in = new TCHAR[g_stringsize];
	int def3 = popint();
	int def2 = popint();
	int def1 = popint();
	popstringn(in, 0);
	popstringn(fmt, 0);
	int out1 = 0;
	int out2 = 0;
	int out3 = 0;
	int result = 0;

#ifdef UNICODE
	result = swscanf(in, fmt, &out1, &out2, &out3);
#else
	result = sscanf(in, fmt, &out1, &out2, &out3);
#endif
	
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

NSISFUNC(SHFileMove)
{
	EXDLL_INIT();
	
	SHFILEOPSTRUCT fileop;
	TCHAR *from = new TCHAR[g_stringsize + 1];
	TCHAR *to = new TCHAR[g_stringsize + 1];

	SecureZeroMemory(&fileop, sizeof(SHFILEOPSTRUCT));
	SecureZeroMemory(from, sizeof(TCHAR) * (g_stringsize + 1));
	SecureZeroMemory(to, sizeof(TCHAR) * (g_stringsize + 1));

	HWND hwnd = (HWND) popint();
	popstring(to);
	popstring(from);

	fileop.hwnd = hwnd;
	fileop.wFunc = FO_MOVE;
	fileop.pFrom = from;
	fileop.pTo = to;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	if(hwnd == 0) fileop.fFlags |= FOF_SILENT;

	int result = SHFileOperation(&fileop);
	pushstring((result == 0) ? (fileop.fAnyOperationsAborted ? _T("ABORTED") : _T("OK")) : _T("ERROR"));

	if((result != 0) && g_bVerbose)
	{
		char temp[1024];
		_snprintf(temp, 1024, "Failed with error code: 0x%X", result);
		temp[1023] = '\0';
		MessageBoxA(NULL, temp, "StdUtils::SHFileMove", MB_TOPMOST|MB_ICONERROR);
	}

	delete [] from;
	delete [] to;
}

NSISFUNC(SHFileCopy)
{
	EXDLL_INIT();
	
	SHFILEOPSTRUCT fileop;
	TCHAR *from = new TCHAR[g_stringsize + 1];
	TCHAR *to = new TCHAR[g_stringsize + 1];

	SecureZeroMemory(&fileop, sizeof(SHFILEOPSTRUCT));
	SecureZeroMemory(from, sizeof(TCHAR) * (g_stringsize + 1));
	SecureZeroMemory(to, sizeof(TCHAR) * (g_stringsize + 1));

	HWND hwnd = (HWND) popint();
	popstring(to);
	popstring(from);

	fileop.hwnd = hwnd;
	fileop.wFunc = FO_COPY;
	fileop.pFrom = from;
	fileop.pTo = to;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	if(hwnd == 0) fileop.fFlags |= FOF_SILENT;

	int result = SHFileOperation(&fileop);
	pushstring((result == 0) ? (fileop.fAnyOperationsAborted ? _T("ABORTED") : _T("OK")) : _T("ERROR"));

	if((result != 0) && g_bVerbose)
	{
		char temp[1024];
		_snprintf(temp, 1024, "Failed with error code: 0x%X", result);
		temp[1023] = '\0';
		MessageBoxA(NULL, temp, "StdUtils::SHFileCopy", MB_TOPMOST|MB_ICONERROR);
	}


	delete [] from;
	delete [] to;
}

NSISFUNC(ExecShellAsUser)
{
	EXDLL_INIT();

	TCHAR *file = new TCHAR[g_stringsize];
	TCHAR *verb = new TCHAR[g_stringsize];
	TCHAR *args = new TCHAR[g_stringsize];

	popstringn(args, 0);
	popstringn(verb, 0);
	popstringn(file, 0);
	
	if(_tcslen(verb) < 1) { delete [] verb; verb = NULL; }
	if(_tcslen(args) < 1) { delete [] args; args = NULL; }

	int result = ShellExecAsUser(verb, file, args, hWndParent, true);
	
	switch(result)
	{
	case 1:
		pushstring(_T("ok"));
		break;
	case 0:
		pushstring(_T("fallback"));
		break;
	case -1:
		pushstring(_T("error"));
		break;
	case -2:
		pushstring(_T("timeout"));
		break;
	default:
		pushstring(_T("unknown"));
		break;
	}

	if(file) delete [] file;
	if(verb) delete [] verb;
	if(args) delete [] args;
}

NSISFUNC(EnableVerboseMode)
{
	EXDLL_INIT();
	g_bVerbose = true;
}

NSISFUNC(DisableVerboseMode)
{
	EXDLL_INIT();
	g_bVerbose = false;
}

NSISFUNC(Unload)
{
	EXDLL_INIT();
}

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		g_bVerbose = false;
		srand(static_cast<unsigned int>(time(NULL)));
	}
	g_hInstance = hInst;
	return TRUE;
}
