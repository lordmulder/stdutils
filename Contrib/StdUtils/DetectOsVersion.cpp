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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "UnicodeSupport.h"
#include "DetectOsVersion.h"

//Forward declaration
static bool verify_os_version(const DWORD major, const DWORD minor);

/*
 * Determine the *real* Windows version
 */
bool get_real_os_version(unsigned int *major, unsigned int *minor, bool *pbOverride)
{
	*major = *minor = 0;
	*pbOverride = false;
	
	//Initialize local variables
	OSVERSIONINFOEXW osvi;
	memset(&osvi, 0, sizeof(OSVERSIONINFOEXW));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

	//Try GetVersionEx() first
	if(GetVersionExW((LPOSVERSIONINFOW)&osvi) == FALSE)
	{
		/*fprintf(stderr, "GetVersionEx() has failed, cannot detect Windows version!\n");*/
		return false;
	}

	//Make sure we are running on NT
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		*major = osvi.dwMajorVersion;
		*minor = osvi.dwMinorVersion;
	}
	else
	{
		/*fprintf(stderr, "Not running on Windows NT, unsupported operating system!\n");*/
		return false;
	}

	//Determine the real *major* version first
	for(;;)
	{
		const DWORD nextMajor = (*major) + 1;
		if(verify_os_version(nextMajor, 0))
		{
			*pbOverride = true;
			*major = nextMajor;
			*minor = 0;
			continue;
		}
		break;
	}

	//Now also determine the real *minor* version
	for(;;)
	{
		const DWORD nextMinor = (*minor) + 1;
		if(verify_os_version((*major), nextMinor))
		{
			*pbOverride = true;
			*minor = nextMinor;
			continue;
		}
		break;
	}

	return true;
}

/*
 * Verify a specific Windows version
 */
static bool verify_os_version(const DWORD major, const DWORD minor)
{
	OSVERSIONINFOEXW osvi;
	DWORDLONG dwlConditionMask = 0;

	//Initialize the OSVERSIONINFOEX structure
	memset(&osvi, 0, sizeof(OSVERSIONINFOEXW));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	osvi.dwMajorVersion = major;
	osvi.dwMinorVersion = minor;
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

	//Initialize the condition mask
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

	// Perform the test
	const BOOL ret = VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID, dwlConditionMask);

	//Error checking
	if(!ret)
	{
		if(GetLastError() != ERROR_OLD_WIN_VERSION)
		{
			/*fprintf(stderr, "VerifyVersionInfo() system call has failed!\n");*/
		}
	}

	return (ret != FALSE);
}

/*
 * Get friendly OS version name
 */
const TCHAR *get_os_friendly_name(const DWORD major, const DWORD minor)
{
	static const size_t NAME_COUNT = 8;

	static const struct
	{
		const DWORD major;
		const DWORD minor;
		const TCHAR name[6];
	}
	s_names[NAME_COUNT] =
	{
		{ 4, 0, T("winnt") },
		{ 5, 0, T("win2k") },
		{ 5, 1, T("winxp") },
		{ 5, 2, T("xpx64") },
		{ 6, 0, T("vista") },
		{ 6, 1, T("win70") },
		{ 6, 2, T("win80") },
		{ 6, 3, T("win81") }
	};

	for(size_t i = 0; i < NAME_COUNT; i++)
	{
		if((s_names[i].major == major) && (s_names[i].minor == minor))
		{
			return &s_names[i].name[0];
		}
	}

	return T("unknown");
}

/*eof*/
