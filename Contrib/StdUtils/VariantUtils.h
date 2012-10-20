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
