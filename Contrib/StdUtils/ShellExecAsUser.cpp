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

#include "ShellExecAsUser.h"
#include "UnicodeSupport.h"
#include "msvc_utils.h"

#include <exdisp.h>
#include <Shobjidl.h>
#include <Shlwapi.h>
#include <SHLGUID.h>

///////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define ALLOC_STRING(STR) SysAllocString(STR)
#else
	static inline BSTR ALLOC_STRING(const char *STR)
	{
		BSTR result = NULL;
		wchar_t *temp = ansi_to_utf16(STR);
		if(temp)
		{
			result = SysAllocString(temp);
			delete [] temp;
		}
		return result;
	}
#endif

class variant_t
{
public:
	variant_t(void) { VariantInit(&data); }
	variant_t(const TCHAR *str) { VariantInit(&data); if(str != NULL) setString(str); }
	variant_t(const LONG value) { VariantInit(&data); setIValue(value); }
	~variant_t(void) { VariantClear(&data); }
	void setIValue(const LONG value) { VariantClear(&data); data.vt = VT_I4; data.lVal = value; }
	void setString(const TCHAR *str) { VariantClear(&data); if(str != NULL) { setOleStr(ALLOC_STRING(str)); } }
	operator const VARIANT&(void) const { return data; };
	operator VARIANT*(void) { return &data; };
	operator const BSTR(void) const { return data.bstrVal; };
protected:
	void setOleStr(const BSTR value) { if(value != NULL) { data.vt = VT_BSTR; data.bstrVal = value; } }
private:
	VARIANT data;
};

typedef struct
{
	const TCHAR *pcOperation;
	const TCHAR *pcFileName;
	const TCHAR *pcParameters;
	const HWND parentHwnd;
	int returnValue;
}
threadParam_t;

///////////////////////////////////////////////////////////////////////////////

static DWORD WINAPI ShellExecAsUser_ThreadHelperProc(LPVOID lpParameter)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if((hr == S_OK) || (hr == S_FALSE))
	{
		threadParam_t *params = (threadParam_t*) lpParameter;
		params->returnValue = ShellExecAsUser(params->pcOperation, params->pcFileName, params->pcParameters, params->parentHwnd, false);
		CoUninitialize();
	}

	return EXIT_SUCCESS;
}

static int ShellExecAsUser_ShellDispatchProc(const TCHAR *pcOperation, const TCHAR *pcFileName, const TCHAR *pcParameters, const HWND parentHwnd)
{
	int iSuccess = -1;

	IShellWindows *psw = NULL;
	HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&psw));
	if(SUCCEEDED(hr))
	{
		HWND hwnd = 0;
		IDispatch* pdisp = NULL;
		variant_t vEmpty;
		if(S_OK == psw->FindWindowSW(vEmpty, vEmpty, SWC_DESKTOP, (long*)&hwnd, SWFO_NEEDDISPATCH, &pdisp))
		{
			if((hwnd != NULL) && (hwnd != INVALID_HANDLE_VALUE))
			{
				IShellBrowser *psb;
				hr = IUnknown_QueryService(pdisp, SID_STopLevelBrowser, IID_PPV_ARGS(&psb));
				if(SUCCEEDED(hr))
				{
					IShellView *psv = NULL;
					hr = psb->QueryActiveShellView(&psv);
					if(SUCCEEDED(hr))
					{
						IDispatch *pdispBackground = NULL;
						HRESULT hr = psv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&pdispBackground));
						if (SUCCEEDED(hr))
						{
							IShellFolderViewDual *psfvd = NULL;
							hr = pdispBackground->QueryInterface(IID_PPV_ARGS(&psfvd));
							if (SUCCEEDED(hr))
							{
								IDispatch *pdisp = NULL;
								hr = psfvd->get_Application(&pdisp);
								if (SUCCEEDED(hr))
								{
									IShellDispatch2 *psd;
									hr = pdisp->QueryInterface(IID_PPV_ARGS(&psd));
									if(SUCCEEDED(hr))
									{
										variant_t verb(pcOperation);
										variant_t file(pcFileName);
										variant_t para(pcParameters);
										variant_t show(SW_SHOWNORMAL);
										hr = psd->ShellExecute(file, para, vEmpty, verb, show);
										if(SUCCEEDED(hr)) iSuccess = 1;
										psd->Release();
										psd = NULL;
									}
									pdisp->Release();
									pdisp = NULL;
								}
							}
							pdispBackground->Release();
							pdispBackground = NULL;
						}
						psv->Release();
						psv = NULL;
					}
					psb->Release();
					psb = NULL;
				}
			}
			pdisp->Release();
			pdisp = NULL;
		}
		psw->Release();
		psw = NULL;
	}

	return iSuccess;
}

int ShellExecAsUser(const TCHAR *pcOperation, const TCHAR *pcFileName, const TCHAR *pcParameters, const HWND parentHwnd, const bool threaded)
{
	int iSuccess = -1;

	OSVERSIONINFO osVersion;
	memset(&osVersion, 0, sizeof(OSVERSIONINFO));
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if(GetVersionEx(&osVersion))
	{
		if((osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osVersion.dwMajorVersion >= 6))
		{
			if(threaded)
			{
				threadParam_t threadParams = {pcOperation, pcFileName, pcParameters, parentHwnd, -1};
				HANDLE hThread = CreateThread(NULL, 0, ShellExecAsUser_ThreadHelperProc, &threadParams, 0, NULL);
				if((hThread != NULL) && (hThread != INVALID_HANDLE_VALUE))
				{
					DWORD status = WaitForSingleObject(hThread, 30000);
					if(status == WAIT_OBJECT_0)
					{
						iSuccess = threadParams.returnValue;
					}
					else if(status == WAIT_TIMEOUT)
					{
						iSuccess = -2;
						TerminateThread(hThread, EXIT_FAILURE);
					}
					CloseHandle(hThread);
					return iSuccess;
				}
			}
			else
			{
				iSuccess = ShellExecAsUser_ShellDispatchProc(pcOperation, pcFileName, pcParameters, parentHwnd);
			}
		}
	}
	
	if(iSuccess < 1)
	{
		HINSTANCE hInst = ShellExecute(parentHwnd, pcOperation, pcFileName, pcParameters, NULL, SW_SHOWNORMAL);
		if(((int)hInst) > 32) iSuccess = 0;
	}

	return iSuccess;
}
