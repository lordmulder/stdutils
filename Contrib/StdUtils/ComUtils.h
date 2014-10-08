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
#ifndef _UNICODE
	variant_t(const WCHAR *str) { VariantInit(&data); if(str != NULL) setString(str); }
	void setString(const WCHAR *str) { VariantClear(&data); if(str != NULL) { setOleStr(SysAllocString(str)); } }
#endif
protected:
	void setOleStr(const BSTR value) { if(value != NULL) { data.vt = VT_BSTR; data.bstrVal = value; } }
private:
	VARIANT data;
};

/*
 * Each single-threaded apartment (STA) must have a message loop to handle calls from other processes and apartments within the same process!
 * We post a quit message, which is generated only when the message queue is otherwise empty. http://blogs.msdn.com/b/oldnewthing/archive/2005/11/04/489028.aspx
 * Then, we process all available messages.
 * Source: http://msdn.microsoft.com/en-us/library/windows/desktop/ms680112%28v=vs.85%29.aspx | http://msdn.microsoft.com/en-us/library/ms809971.aspx
 */
static void DispatchPendingMessages()
{
	MSG msg;

	PostQuitMessage(0);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*eof*/
