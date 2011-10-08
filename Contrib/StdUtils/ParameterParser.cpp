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

#include <Windows.h>
#include "UnicodeSupport.h"

static bool parse_parameter(const TCHAR *str, const size_t len, const TCHAR *arg_name, bool *first, TCHAR *dest_buff, size_t dest_size)
{
	if(*first)
	{
		*first = false;
		return false;
	}

	bool bSuccess = false;
	
	if((len > 1) && (str[0] == L'/'))
	{
		TCHAR *buffer = new TCHAR[len];
		memset(buffer, 0, sizeof(TCHAR) * len);
		STRNCPY(buffer, &str[1], len-1);

		TCHAR *offset = STRCHR(buffer, T('='));
		if(offset != NULL)
		{
			offset[0] = L'\0';
			if(STRICMP(buffer, arg_name) == 0)
			{
				size_t right = STRLEN(&offset[1]);
				while((right > 0) && (offset[right] == T(' ')))
				{
					offset[right] = '\0';
					right--;
				};
				size_t left = 1;
				while(offset[left] == T(' ')) left++;
				STRNCPY(dest_buff, &offset[left], dest_size);
				dest_buff[dest_size-1] = L'\0';
				bSuccess = true;
			}
		}
		else
		{
			if(STRICMP(buffer, arg_name) == 0)
			{
				dest_buff[0] = L'\0';
				bSuccess = true;
			}
		}
	
		delete [] buffer;
	}

	return bSuccess;
}

bool parse_commandline(const TCHAR *arg_name, TCHAR *dest_buff, size_t dest_size)
{
	bool bSuccess = false;
	TCHAR *cmd = GetCommandLine();
	
	if(cmd)
	{
		bool first = true;
		size_t cmd_len = STRLEN(cmd);
		size_t tok_len = 0;
		TCHAR *tok_pos = NULL;
		bool flag = false;
		for(size_t i = 0; i < cmd_len; i++)
		{
			if(cmd[i] == L'\"')
			{
				if(tok_pos != NULL)
				{
					if(parse_parameter(tok_pos, tok_len, arg_name, &first, dest_buff, dest_size))
					{
						bSuccess = true;
					}
				}
				tok_len = 0;
				tok_pos = NULL;
				flag = !flag;
				continue;
			}
			if((cmd[i] == L' ') && (flag == false))
			{
				if(tok_pos != NULL)
				{
					if(parse_parameter(tok_pos, tok_len, arg_name, &first, dest_buff, dest_size))
					{
						bSuccess = true;
					}
				}
				tok_len = 0;
				tok_pos = NULL;
				continue;
			}
			if(tok_pos == NULL)
			{
				tok_pos = &cmd[i];
			}
			tok_len++;
		}
		if(tok_pos != NULL)
		{
			if(parse_parameter(tok_pos, tok_len, arg_name, &first, dest_buff, dest_size))
			{
				bSuccess = true;
			}
		}
	}

	return bSuccess;
}
