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

#pragma warning(disable:4996)

#include <Windows.h>
#include "UnicodeSupport.h"

static bool parse_parameter(TCHAR *const buffer, const TCHAR *const arg_name, bool *const first, TCHAR *dest_buff, size_t dest_size)
{
	if(*first)
	{
		*first = false;
		return false;
	}

	TCHAR *offset = STRCHR(buffer, T('='));
	if(!offset)
	{
		if((buffer[0] == T('/')) && (STRICMP(STRTRIM(&buffer[1]), arg_name) == 0))
		{
			dest_buff[0] = T('\0');
			return true;
		}
		return false;
	}

	*(offset++) = T('\0');
	if((buffer[0] == T('/')) && (STRICMP(STRTRIM(&buffer[1]), arg_name) == 0))
	{
		STRNCPY(dest_buff, STRTRIM(offset), dest_size);
		dest_buff[dest_size-1] = T('\0');
		return true;
	}
	
	return false;
}

bool parse_commandline(const TCHAR *const arg_name, TCHAR *const dest_buff, size_t dest_size)
{
	if((!arg_name) || (!arg_name[0]))
	{
		return false;
	}

	const TCHAR *const cmd = GetCommandLine();
	if((!cmd) || (!cmd[0]))
	{
		return false;
	}

	size_t pos = 0, tok_len = 0;
	bool first = true;
	TCHAR *buffer = new TCHAR[STRLEN(cmd) + 1];

	while(cmd[pos])
	{
		bool flag = false;
		while(WHITESPACE(cmd[pos])) pos++;
		while(cmd[pos])
		{
			if(cmd[pos] == T('"'))
			{
				flag = (!flag);
			}
			else
			{
				const bool is_space = WHITESPACE(cmd[pos]);
				if((!flag) && is_space)
				{
					if(tok_len > 0)
					{
						buffer[tok_len] = T('\0');
						if(parse_parameter(buffer, arg_name, &first, dest_buff, dest_size))
						{
							delete [] buffer;
							return true;
						}
						tok_len = 0;
					}
					break;
				}
				buffer[tok_len++] = is_space ? T(' ') : cmd[pos];
			}
			pos++;
		}
	}

	if(tok_len > 0)
	{
		buffer[tok_len] = T('\0');
		if(parse_parameter(buffer, arg_name, &first, dest_buff, dest_size))
		{
			delete [] buffer;
			return true;
		}
	}

	delete [] buffer;
	return false;
}

const TCHAR *get_commandline_arguments(void)
{
	const TCHAR *cmd = GetCommandLine();
	if((!cmd) || (!cmd[0]))
	{
		static const TCHAR *error = T("error");
		return error;
	}

	size_t pos = 0;
	while(WHITESPACE(cmd[pos])) pos++;

	bool flag = false;
	while(cmd[pos])
	{
		if(cmd[pos] == T('"'))
		{
			flag = (!flag);
		}
		else
		{
			if((!flag) && WHITESPACE(cmd[pos]))
			{
				break;
			}
		}
		pos++;
	}

	while(WHITESPACE(cmd[pos])) pos++;
	return &cmd[pos];
}
