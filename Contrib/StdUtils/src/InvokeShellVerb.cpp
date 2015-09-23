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

///////////////////////////////////////////////////////////////////////////////
// The following code was strongly inspired by the "InvokeShellVerb" plug-in
// Copyright (c) 2011 Robert Strong
// For details see: http://nsis.sourceforge.net/Invoke_Shell_Verb_plugin
///////////////////////////////////////////////////////////////////////////////

#include "InvokeShellVerb.h"
#include "ShellDispatch.h"

///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	const DWORD uiVerbId;
	const TCHAR *const pcDirectoryName;
	const TCHAR *const pcFileName;
}
invoke_shellverb_param_t;

static const WCHAR *shell32 = L"shell32.dll";

///////////////////////////////////////////////////////////////////////////////

static int MyInvokeShellVerb_HandlerProc(IShellDispatch2 *const dispatch, const void *const data)
{
	int iSuccess = INVOKE_SHELLVERB_FAILED;
	const invoke_shellverb_param_t *const param = (const invoke_shellverb_param_t*) data;

	bool bUnloadDll = false;
	HMODULE hShellDll = GetModuleHandleW(shell32); 
	if(hShellDll == NULL)
	{
		bUnloadDll = true;
		hShellDll = LoadLibraryW(shell32);
		if(hShellDll == NULL)
		{
			return iSuccess;
		}
	}

	WCHAR pcVerbName[128];
	memset(pcVerbName, 0, sizeof(WCHAR) * 128);
	
	if(LoadStringW(hShellDll, param->uiVerbId, pcVerbName, 128) < 1)
	{
		if(bUnloadDll)
		{
			FreeLibrary(hShellDll);
			hShellDll = NULL;
		}
		return iSuccess;
	}

	if(bUnloadDll)
	{
		FreeLibrary(hShellDll);
		hShellDll = NULL;
	}

	// ----------------------------------- //

	Folder *pFolder = NULL; FolderItem *pItem = NULL;

	variant_t vaDirectory(param->pcDirectoryName);
	HRESULT hr = dispatch->NameSpace(vaDirectory, &pFolder);
	if(FAILED(hr) || (pFolder == NULL))
	{
		return (iSuccess = INVOKE_SHELLVERB_NOT_FOUND);
	}

	variant_t vaFileName(param->pcFileName);
	hr = pFolder->ParseName(vaFileName, &pItem);
	if(FAILED(hr) || (pItem == NULL))
	{
		RELEASE_OBJ(pFolder);
		return (iSuccess = INVOKE_SHELLVERB_NOT_FOUND);
	}

	RELEASE_OBJ(pFolder);

	// ----------------------------------- //

	long iVerbCount = 0;
	FolderItemVerbs *pVerbs = NULL;
	
	hr = pItem->Verbs(&pVerbs);
	if(FAILED(hr) || (pVerbs == NULL))
	{
		RELEASE_OBJ(pItem);
		return iSuccess;
	}

	RELEASE_OBJ(pItem);

	hr = pVerbs->get_Count(&iVerbCount);
	if(FAILED(hr) || (iVerbCount < 1))
	{
		RELEASE_OBJ(pVerbs);
		return iSuccess;
	}

	DispatchPendingMessages(125);

	// ----------------------------------- //

	for(int i = 0; i < iVerbCount; i++)
	{
		variant_t vaVariantIndex(i);
		FolderItemVerb *pCurrentVerb = NULL;
		BSTR pcCurrentVerbName = NULL;

		hr = pVerbs->Item(vaVariantIndex, &pCurrentVerb);
		if (FAILED(hr) || (pCurrentVerb == NULL))
		{
			continue;
		}
		
		hr = pCurrentVerb->get_Name(&pcCurrentVerbName);
		if(FAILED(hr) || (pcCurrentVerbName == NULL))
		{
			RELEASE_OBJ(pCurrentVerb);
			continue;
		}

		if(_wcsicmp(pcCurrentVerbName, pcVerbName) == 0)
		{
			hr = pCurrentVerb->DoIt();
			if(!FAILED(hr))
			{
				iSuccess = INVOKE_SHELLVERB_SUCCESS;
			}
		}

		SysFreeString(pcCurrentVerbName);
		RELEASE_OBJ(pCurrentVerb);

		if(iSuccess == INVOKE_SHELLVERB_SUCCESS)
		{
			break; /*succeeded*/
		}
	}

	RELEASE_OBJ(pVerbs);

	// ----------------------------------- //
	
	return iSuccess;
}

int MyInvokeShellVerb(const TCHAR *pcDirectoryName, const TCHAR *pcFileName, const DWORD uiVerbId, const bool threaded)
{
	int iSuccess = INVOKE_SHELLVERB_FAILED;

	OSVERSIONINFO osVersion;
	memset(&osVersion, 0, sizeof(OSVERSIONINFO));
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if(GetVersionEx(&osVersion))
	{
		if((osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) && ((osVersion.dwMajorVersion > 6) || ((osVersion.dwMajorVersion == 6) && (osVersion.dwMinorVersion >= 1))))
		{
			invoke_shellverb_param_t params = { uiVerbId, pcDirectoryName, pcFileName };
			iSuccess = MyShellDispatch(MyInvokeShellVerb_HandlerProc, &params, threaded);
		}
		else
		{
			iSuccess = INVOKE_SHELLVERB_UNSUPPORTED;
		}
	}

	return iSuccess;
}
