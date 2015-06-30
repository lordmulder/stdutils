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

#include "Mutex.h"
#include "UnicodeSupport.h"
#include <stdlib.h>
#include <msvc_utils.h>

//Global
extern RTL_CRITICAL_SECTION g_pStdUtilsMutex;

//Stdlib
typedef struct { int newmode; } _startupinfo;
extern "C"
{
	int __getmainargs (int *_Argc, char    ***_Argv, char    ***_Env, int _DoWildCard, _startupinfo *_StartInfo);
	int __wgetmainargs(int *_Argc, wchar_t ***_Argv, wchar_t ***_Env, int _DoWildCard, _startupinfo *_StartInfo);
}

//Unicode support
#ifdef UNICODE
#define GETMAINARGS __wgetmainargs
#else
#define GETMAINARGS __getmainargs
#endif

//Command-line parameters buffer
static int s_argc = 0;
static TCHAR **s_argv = NULL, **s_envp = NULL;

static bool init_mainargs(void)
{
	MutexLocker locker(&g_pStdUtilsMutex);
	if(!s_argv)
	{
		_startupinfo si = { 0 };
		if(GETMAINARGS(&s_argc, &s_argv, &s_envp, 0, &si) == 0)
		{
			return (s_argv != NULL);
		}
		return false;
	}
	return true;
}

static bool try_parse_parameter(const TCHAR *const argstr, const TCHAR *const arg_name, const size_t arg_name_len, TCHAR *const dest_buff, const size_t dest_size)
{
	const TCHAR *separator = STRCHR(argstr, T('='));
	if(!separator)
	{
		if((argstr[0] == T('/')) && (STRICMP(&argstr[1], arg_name) == 0))
		{
			if(dest_buff && (dest_size > 0))
			{
				dest_buff[0] = T('\0');
			}
			return true;
		}
		return false;
	}

	const size_t len = separator - argstr;
	if(len > arg_name_len)
	{
		if((argstr[0] == T('/')) && (STRNICMP(&argstr[1], arg_name, arg_name_len) == 0))
		{
			if(dest_buff && (dest_size > 0))
			{
				STRNCPY(dest_buff, ++separator, dest_size);
				dest_buff[dest_size-1] = T('\0');
			}
			return true;
		}
	}
	
	return false;
}

bool parse_commandline_param(const TCHAR *const arg_name, TCHAR *const dest_buff, const size_t dest_size)
{
	if(init_mainargs())
	{
		for(int i = 1; i < s_argc; i++)
		{
			const size_t arg_name_len = STRLEN(arg_name);
			if(try_parse_parameter(s_argv[i], arg_name, arg_name_len, dest_buff, dest_size))
			{
				return true;
			}
		}
	}

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

void free_commandline_args(void)
{
	MutexLocker locker(&g_pStdUtilsMutex);
	if(s_argv)
	{
		for (TCHAR **ptr = s_argv; (*ptr); ++ptr)
		{
			free(*ptr);
		}
		free(s_argv);
		s_argv = NULL;
		s_argc = 0;
	}
}
