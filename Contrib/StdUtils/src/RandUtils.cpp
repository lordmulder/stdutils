///////////////////////////////////////////////////////////////////////////////
// StdUtils plug-in for NSIS
// Copyright (C) 2004-2017 LoRd_MuldeR <MuldeR2@GMX.de>
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

#include "RandUtils.h"

#include "Mutex.h"
#include "UnicodeSupport.h"
#include "msvc_utils.h"
#include <intrin.h> 

//Prototype
typedef BOOLEAN (__stdcall *TRtlGenRandom)(PVOID RandomBuffer, ULONG RandomBufferLength);

//State
static volatile long s_secure_rand_init = 0;
static TRtlGenRandom s_pRtlGenRandom = NULL;

//Helper macro
#define RANDU32() (((static_cast<unsigned int>(rand()) & 0xFFFF) << 16) | (static_cast<unsigned int>(rand()) & 0xFFFF))

/*initialize rand*/
static __forceinline void init_rand(void)
{
	long state;
	while((state = _InterlockedCompareExchange( &s_secure_rand_init, -1, 0 )) != 0)
	{
		if(state > 0)
		{
			return; /*already initialized*/
		}
	}

	srand(static_cast<unsigned int>(time(NULL)));
	if(const HMODULE advapi32 = GetModuleHandle(T("Advapi32.dll")))
	{
		s_pRtlGenRandom = reinterpret_cast<TRtlGenRandom>(GetProcAddress(advapi32, "SystemFunction036"));
	}

	_InterlockedExchange(&s_secure_rand_init, 1);
}

/* Robert Jenkins' 96 bit Mix Function */
static inline unsigned int mix_function(unsigned int a, unsigned int b, unsigned int c)
{
	a=a-b;  a=a-c;  a=a^(c >> 13);
	b=b-c;  b=b-a;  b=b^(a <<  8);
	c=c-a;  c=c-b;  c=c^(b >> 13);
	a=a-b;  a=a-c;  a=a^(c >> 12);
	b=b-c;  b=b-a;  b=b^(a << 16);
	c=c-a;  c=c-b;  c=c^(b >>  5);
	a=a-b;  a=a-c;  a=a^(c >>  3);
	b=b-c;  b=b-a;  b=b^(a << 10);
	c=c-a;  c=c-b;  c=c^(b >> 15);
	return c;
}

//32-Bit rand() version
static inline unsigned int fallback_rand()
{
	return mix_function(RANDU32(), RANDU32(), RANDU32());
}

unsigned int next_rand(void)
{
	init_rand();

	if(s_pRtlGenRandom)
	{
		unsigned int rnd;
		if(s_pRtlGenRandom(&rnd, sizeof(unsigned int)))
		{
			return rnd;
		}
	}

	/*fallback implementation*/
	return fallback_rand();
}

void rand_bytes(unsigned char *const buffer, const size_t size)
{
	init_rand();

	if(s_pRtlGenRandom)
	{
		if(s_pRtlGenRandom(buffer, size))
		{
			return; /*success*/
		}
	}

	/*fallback implementation*/
	unsigned int rnd;
	for(size_t i = 0; i < size; ++i)
	{
		if(!(i & 3U))
		{
			rnd = fallback_rand();
		}
		buffer[i] = (unsigned char)(rnd & 0xFF);
		rnd >>= 8;
	}
}
