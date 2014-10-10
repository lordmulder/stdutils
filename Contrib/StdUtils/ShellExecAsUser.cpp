///////////////////////////////////////////////////////////////////////////////
// StdUtils plug-in for NSIS
// Copyright (C) 2004-2014 LoRd_MuldeR <MuldeR2@GMX.de>
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

#include "ShellExecAsUser.h"
#include "UnicodeSupport.h"
#include "msvc_utils.h"

#include <exdisp.h>
#include <Shobjidl.h>
#include <Shlwapi.h>
#include <SHLGUID.h>

///////////////////////////////////////////////////////////////////////////////

#include "ComUtils.h"

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

static unsigned __stdcall ShellExecAsUser_ThreadHelperProc(void* pArguments)
{
	HRESULT hr = CoInitialize(NULL);
	if((hr == S_OK) || (hr == S_FALSE))
	{
		if(threadParam_t *params = (threadParam_t*) pArguments)
		{
			params->returnValue = ShellExecAsUser(params->pcOperation, params->pcFileName, params->pcParameters, params->parentHwnd, false);
		}
		DispatchPendingMessages(1000); //Required to avoid potential deadlock or crash on CoUninitialize() !!!
		CoUninitialize();
	}
	else
	{
		if(threadParam_t *params = (threadParam_t*) pArguments)
		{
			params->returnValue = SHELLEXECASUSER_ERROR_FAILED;
		}
	}

	return EXIT_SUCCESS;
}

static int ShellExecAsUser_ShellDispatchProc_ShellExecute(IShellFolderViewDual *const psfvd, const TCHAR *const pcOperation, const TCHAR *const pcFileName, const TCHAR *const pcParameters)
{
	int iSuccess = SHELLEXECASUSER_ERROR_FAILED;

	IDispatch *pdisp = NULL;
	HRESULT hr = psfvd->get_Application(&pdisp);
	if(SUCCEEDED(hr))
	{
		IShellDispatch2 *psd;
		hr = pdisp->QueryInterface(IID_PPV_ARGS(&psd));
		if(SUCCEEDED(hr))
		{
			DispatchPendingMessages(125);
			variant_t vEmpty;
			variant_t verb(pcOperation);
			variant_t file(pcFileName);
			variant_t para(pcParameters);
			variant_t show(SW_SHOWNORMAL);
			hr = psd->ShellExecute(file, para, vEmpty, verb, show);
			if(SUCCEEDED(hr))
			{
				iSuccess = SHELLEXECASUSER_ERROR_SUCCESS;
			}
			psd->Release();
			psd = NULL;
		}
		pdisp->Release();
		pdisp = NULL;
	}

	return iSuccess;
}

static int ShellExecAsUser_ShellDispatchProc(const TCHAR *const pcOperation, const TCHAR *const pcFileName, const TCHAR *const pcParameters, const HWND &parentHwnd)
{
	int iSuccess = SHELLEXECASUSER_ERROR_FAILED;

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
							if(SUCCEEDED(hr))
							{
								iSuccess = ShellExecAsUser_ShellDispatchProc_ShellExecute(psfvd, pcOperation, pcFileName, pcParameters);
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

int ShellExecAsUser(const TCHAR *const pcOperation, const TCHAR *const pcFileName, const TCHAR *const pcParameters, const HWND &parentHwnd, const bool &threaded)
{
	//Make sure the destination file exists
	if(GetFileAttributes(pcFileName) == INVALID_FILE_ATTRIBUTES)
	{
		return SHELLEXECASUSER_ERROR_NOTFOUND;
	}

	OSVERSIONINFO osVersion;
	memset(&osVersion, 0, sizeof(OSVERSIONINFO));
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	//Use IShellDispatch2 on supported platforms
	if(GetVersionEx(&osVersion))
	{
		if((osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osVersion.dwMajorVersion >= 6))
		{
			int iSuccess = SHELLEXECASUSER_ERROR_FAILED;
			if(threaded)
			{
				threadParam_t threadParams = {pcOperation, pcFileName, pcParameters, parentHwnd, -1};
				HANDLE hThread = (HANDLE) _beginthreadex(NULL, 0, ShellExecAsUser_ThreadHelperProc, &threadParams, 0, NULL);
				if((hThread != NULL) && (hThread != INVALID_HANDLE_VALUE))
				{
					DWORD status = WaitForSingleObject(hThread, 30000);
					if(status == WAIT_OBJECT_0)
					{
						iSuccess = threadParams.returnValue;
					}
					else if(status == WAIT_TIMEOUT)
					{
						iSuccess = SHELLEXECASUSER_ERROR_TIMEOUT;
						TerminateThread(hThread, EXIT_FAILURE);
					}
					CloseHandle(hThread);
				}
			}
			else
			{
				iSuccess = ShellExecAsUser_ShellDispatchProc(pcOperation, pcFileName, pcParameters, parentHwnd);
			}
			return iSuccess;
		}
	}
	
	//Fallback mode
	HINSTANCE hInst = ShellExecute(parentHwnd, pcOperation, pcFileName, pcParameters, NULL, SW_SHOWNORMAL);
	if(((int)hInst) > 32)
	{
		return SHELLEXECASUSER_ERROR_FALLBACK;
	}

	return SHELLEXECASUSER_ERROR_FAILED;
}
