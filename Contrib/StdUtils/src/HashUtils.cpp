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

//SHA3
#include "rhash/sha3.h"
#include "HashUtils.h"

//Win32
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif

//Internal
#include "WinUtils.h"
#include "UnicodeSupport.h"

//Byte to Hex
static const TCHAR *const HEX_CONVERT[256] =
{
	T("00"), T("01"), T("02"), T("03"), T("04"), T("05"), T("06"), T("07"),
	T("08"), T("09"), T("0A"), T("0B"), T("0C"), T("0D"), T("0E"), T("0F"),
	T("10"), T("11"), T("12"), T("13"), T("14"), T("15"), T("16"), T("17"),
	T("18"), T("19"), T("1A"), T("1B"), T("1C"), T("1D"), T("1E"), T("1F"),
	T("20"), T("21"), T("22"), T("23"), T("24"), T("25"), T("26"), T("27"),
	T("28"), T("29"), T("2A"), T("2B"), T("2C"), T("2D"), T("2E"), T("2F"),
	T("30"), T("31"), T("32"), T("33"), T("34"), T("35"), T("36"), T("37"),
	T("38"), T("39"), T("3A"), T("3B"), T("3C"), T("3D"), T("3E"), T("3F"),
	T("40"), T("41"), T("42"), T("43"), T("44"), T("45"), T("46"), T("47"),
	T("48"), T("49"), T("4A"), T("4B"), T("4C"), T("4D"), T("4E"), T("4F"),
	T("50"), T("51"), T("52"), T("53"), T("54"), T("55"), T("56"), T("57"),
	T("58"), T("59"), T("5A"), T("5B"), T("5C"), T("5D"), T("5E"), T("5F"),
	T("60"), T("61"), T("62"), T("63"), T("64"), T("65"), T("66"), T("67"),
	T("68"), T("69"), T("6A"), T("6B"), T("6C"), T("6D"), T("6E"), T("6F"),
	T("70"), T("71"), T("72"), T("73"), T("74"), T("75"), T("76"), T("77"),
	T("78"), T("79"), T("7A"), T("7B"), T("7C"), T("7D"), T("7E"), T("7F"),
	T("80"), T("81"), T("82"), T("83"), T("84"), T("85"), T("86"), T("87"),
	T("88"), T("89"), T("8A"), T("8B"), T("8C"), T("8D"), T("8E"), T("8F"),
	T("90"), T("91"), T("92"), T("93"), T("94"), T("95"), T("96"), T("97"),
	T("98"), T("99"), T("9A"), T("9B"), T("9C"), T("9D"), T("9E"), T("9F"),
	T("A0"), T("A1"), T("A2"), T("A3"), T("A4"), T("A5"), T("A6"), T("A7"),
	T("A8"), T("A9"), T("AA"), T("AB"), T("AC"), T("AD"), T("AE"), T("AF"),
	T("B0"), T("B1"), T("B2"), T("B3"), T("B4"), T("B5"), T("B6"), T("B7"),
	T("B8"), T("B9"), T("BA"), T("BB"), T("BC"), T("BD"), T("BE"), T("BF"),
	T("C0"), T("C1"), T("C2"), T("C3"), T("C4"), T("C5"), T("C6"), T("C7"),
	T("C8"), T("C9"), T("CA"), T("CB"), T("CC"), T("CD"), T("CE"), T("CF"),
	T("D0"), T("D1"), T("D2"), T("D3"), T("D4"), T("D5"), T("D6"), T("D7"),
	T("D8"), T("D9"), T("DA"), T("DB"), T("DC"), T("DD"), T("DE"), T("DF"),
	T("E0"), T("E1"), T("E2"), T("E3"), T("E4"), T("E5"), T("E6"), T("E7"),
	T("E8"), T("E9"), T("EA"), T("EB"), T("EC"), T("ED"), T("EE"), T("EF"),
	T("F0"), T("F1"), T("F2"), T("F3"), T("F4"), T("F5"), T("F6"), T("F7"),
	T("F8"), T("F9"), T("FA"), T("FB"), T("FC"), T("FD"), T("FE"), T("FF"),
};

static size_t GetHashSize(const int hashType)
{
	switch(hashType)
	{
		case STD_HASHTYPE_SHA3_224: return sha3_224_hash_size;
		case STD_HASHTYPE_SHA3_256: return sha3_256_hash_size;
		case STD_HASHTYPE_SHA3_384: return sha3_384_hash_size;
		case STD_HASHTYPE_SHA3_512: return sha3_512_hash_size;
		default: return SIZE_MAX;
	}
}

static size_t InitHashContext(const int hashType, sha3_ctx *const ctx)
{
	switch(hashType)
	{
		case STD_HASHTYPE_SHA3_224: rhash_sha3_224_init(ctx); return true;
		case STD_HASHTYPE_SHA3_256: rhash_sha3_256_init(ctx); return true;
		case STD_HASHTYPE_SHA3_384: rhash_sha3_384_init(ctx); return true;
		case STD_HASHTYPE_SHA3_512: rhash_sha3_512_init(ctx); return true;
		default: return false;
	}
}

static void ConvertHashToHex(const size_t hashSize, const BYTE *const hashValue, TCHAR *const hashOut)
{
	size_t offset = 0;
	for(size_t i = 0; i < hashSize; i++)
	{
		hashOut[offset++] = HEX_CONVERT[hashValue[i]][0];
		hashOut[offset++] = HEX_CONVERT[hashValue[i]][1];
	}
	hashOut[offset] = T('\0');
}

bool ComputeHash_FromFile(const int hashType, const TCHAR *const fileName, TCHAR *const hashOut, const size_t hashOutSize)
{
	const size_t hashSize = GetHashSize(hashType);
	if((hashSize == SIZE_MAX) || (hashSize >= (hashOutSize / 2)))
	{
		return false;
	}

	HANDLE h = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(!VALID_HANDLE(h))
	{
		return false;
	}

	sha3_ctx ctx;
	if(!InitHashContext(hashType, &ctx))
	{
		CLOSE_HANDLE(h);
		return false;
	}

	static const DWORD BUFF_SIZE = 4096;
	BYTE buffer[BUFF_SIZE];

	for(;;)
	{
		DWORD nBytesRead = 0;
		if(!ReadFile(h, buffer, BUFF_SIZE, &nBytesRead, NULL))
		{
			CLOSE_HANDLE(h);
			return false;
		}
		if(nBytesRead < 1)
		{
			CLOSE_HANDLE(h);
			break;
		}
		rhash_sha3_update(&ctx, buffer, nBytesRead);
	}

	BYTE hashValue[64];
	rhash_sha3_final(&ctx, hashValue);

	ConvertHashToHex(hashSize, hashValue, hashOut);
	return true;
}

bool ComputeHash_FromText(const int hashType, const TCHAR *const textData, TCHAR *const hashOut, const size_t hashOutSize)
{
	const size_t hashSize = GetHashSize(hashType);
	if((hashSize == SIZE_MAX) || (hashSize >= (hashOutSize / 2)))
	{
		return false;
	}

	sha3_ctx ctx;
	if(!InitHashContext(hashType, &ctx))
	{
		return false;
	}

#ifdef UNICODE
	const char *const textUtf8 = utf16_to_utf8(textData);
	if(textUtf8 == NULL)
	{
		return false;
	}
	rhash_sha3_update(&ctx, (BYTE*)textUtf8, strlen(textUtf8));
	delete [] textUtf8;
#else
	rhash_sha3_update(&ctx, (BYTE*)textData, strlen(textData));
#endif

	BYTE hashValue[64];
	rhash_sha3_final(&ctx, hashValue);

	ConvertHashToHex(hashSize, hashValue, hashOut);
	return true;
};
