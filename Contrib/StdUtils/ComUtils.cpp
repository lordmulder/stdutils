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

#include <exdisp.h>
#include <Shobjidl.h>
#include <Shlwapi.h>
#include <SHLGUID.h>

void DisptachPendingMessages(const DWORD uiDelay)
{
	const DWORD uiTimeout = GetTickCount() + (20 * uiDelay);
	const HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(hEvent)
	{
		for(;;)
		{
			MSG poMessage;
			while(PeekMessage(&poMessage, NULL, WM_NULL, WM_NULL, PM_REMOVE))
			{
				TranslateMessage(&poMessage);
				DispatchMessage(&poMessage);
			}
			const DWORD uiResult = MsgWaitForMultipleObjects(1, &hEvent, FALSE, uiDelay, QS_ALLINPUT | QS_ALLPOSTMESSAGE);
			if((uiResult == WAIT_TIMEOUT) || (uiResult == WAIT_FAILED) || (GetTickCount() >= uiTimeout)) break;
		}
		CloseHandle(hEvent);
	}
}
