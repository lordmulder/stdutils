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

#include "HashUtils.h"

//RHash
#include "rhash/crc32.h"
#include "rhash/md5.h"
#include "rhash/sha1.h"
#include "rhash/sha256.h"
#include "rhash/sha512.h"
#include "rhash/sha3.h"

//Blake2
#include "blake2/blake2.h"

//Win32
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif

//Internal
#include "WinUtils.h"
#include "UnicodeSupport.h"

//Const
static const uint32_t CRC32_INIT_VECTOR = 0x00000000;

//Byte to Hex
extern const TCHAR *const LUT_BYTE2HEX[256];

//Hash context
ALIGN(64) typedef union hash_ctx
{
	uint32_t   crc32;
	md5_ctx    md5;
	sha1_ctx   sha1;
	sha256_ctx sha256;
	sha512_ctx sha512;
	sha3_ctx   sha3;
	blake2_ctx balke2;
}
hash_ctx;

static size_t GetHashSize(const int hashType)
{
	switch(hashType)
	{
		case STD_HASHTYPE_CRC_32:   return sizeof(uint32_t);
		case STD_HASHTYPE_MD5_128:  return md5_hash_size;
		case STD_HASHTYPE_SHA1_160: return sha1_hash_size;
		case STD_HASHTYPE_SHA2_224: return sha224_hash_size;
		case STD_HASHTYPE_SHA2_256: return sha256_hash_size;
		case STD_HASHTYPE_SHA2_384: return sha384_hash_size;
		case STD_HASHTYPE_SHA2_512: return sha512_hash_size;
		case STD_HASHTYPE_SHA3_224: return sha3_224_hash_size;
		case STD_HASHTYPE_SHA3_256: return sha3_256_hash_size;
		case STD_HASHTYPE_SHA3_384: return sha3_384_hash_size;
		case STD_HASHTYPE_SHA3_512: return sha3_512_hash_size;
		case STD_HASHTYPE_BLK2_224: return blk2_224_hash_size;
		case STD_HASHTYPE_BLK2_256: return blk2_256_hash_size;
		case STD_HASHTYPE_BLK2_384: return blk2_384_hash_size;
		case STD_HASHTYPE_BLK2_512: return blk2_512_hash_size;
		default: return SIZE_MAX;
	}
}

static inline bool HashFunction_Init(const int hashType, hash_ctx *const ctx)
{
	memset(ctx, 0, sizeof(hash_ctx));
	switch(hashType)
	{
		case STD_HASHTYPE_CRC_32:   rhash_crc32_init(&ctx->crc32);                   return true;
		case STD_HASHTYPE_MD5_128:  rhash_md5_init(&ctx->md5);                       return true;
		case STD_HASHTYPE_SHA1_160: rhash_sha1_init(&ctx->sha1);                     return true;
		case STD_HASHTYPE_SHA2_224: rhash_sha224_init(&ctx->sha256);                 return true;
		case STD_HASHTYPE_SHA2_256: rhash_sha256_init(&ctx->sha256);                 return true;
		case STD_HASHTYPE_SHA2_384: rhash_sha384_init(&ctx->sha512);                 return true;
		case STD_HASHTYPE_SHA2_512: rhash_sha512_init(&ctx->sha512);                 return true;
		case STD_HASHTYPE_SHA3_224: rhash_sha3_224_init(&ctx->sha3);                 return true;
		case STD_HASHTYPE_SHA3_256: rhash_sha3_256_init(&ctx->sha3);                 return true;
		case STD_HASHTYPE_SHA3_384: rhash_sha3_384_init(&ctx->sha3);                 return true;
		case STD_HASHTYPE_SHA3_512: rhash_sha3_512_init(&ctx->sha3);                 return true;
		case STD_HASHTYPE_BLK2_224: return (blake2b_init(&ctx->balke2, blk2_224_hash_size) == 0);
		case STD_HASHTYPE_BLK2_256: return (blake2b_init(&ctx->balke2, blk2_256_hash_size) == 0);
		case STD_HASHTYPE_BLK2_384: return (blake2b_init(&ctx->balke2, blk2_384_hash_size) == 0);
		case STD_HASHTYPE_BLK2_512: return (blake2b_init(&ctx->balke2, blk2_512_hash_size) == 0);
		default: return false;
	}
}

static inline bool HashFunction_Update(const int hashType, hash_ctx *const ctx, const unsigned char *const msg, const size_t size)
{
	switch(hashType)
	{
		case STD_HASHTYPE_CRC_32:
			rhash_crc32_update(&ctx->crc32, msg, size);
			return true;
		case STD_HASHTYPE_MD5_128:
			rhash_md5_update(&ctx->md5, msg, size);
			return true;
		case STD_HASHTYPE_SHA1_160:
			rhash_sha1_update(&ctx->sha1, msg, size);
			return true;
		case STD_HASHTYPE_SHA2_224:
		case STD_HASHTYPE_SHA2_256:
			rhash_sha256_update(&ctx->sha256, msg, size);
			return true;
		case STD_HASHTYPE_SHA2_384:
		case STD_HASHTYPE_SHA2_512:
			rhash_sha512_update(&ctx->sha512, msg, size);
			return true;
		case STD_HASHTYPE_SHA3_224:
		case STD_HASHTYPE_SHA3_256:
		case STD_HASHTYPE_SHA3_384:
		case STD_HASHTYPE_SHA3_512:
			rhash_sha3_update(&ctx->sha3, msg, size);
			return true;
		case STD_HASHTYPE_BLK2_224:
		case STD_HASHTYPE_BLK2_256:
		case STD_HASHTYPE_BLK2_384:
		case STD_HASHTYPE_BLK2_512:
			return (blake2b_update(&ctx->balke2, msg, size) == 0);
		default:
			return false;
	}
}

static inline bool HashFunction_Final(const int hashType, hash_ctx *const ctx, unsigned char *const result)
{
	switch(hashType)
	{
		case STD_HASHTYPE_CRC_32:
			rhash_crc32_final(&ctx->crc32, result);
			return true;
		case STD_HASHTYPE_MD5_128:
			rhash_md5_final(&ctx->md5, result);
			return true;
		case STD_HASHTYPE_SHA1_160:
			rhash_sha1_final(&ctx->sha1, result);
			return true;
		case STD_HASHTYPE_SHA2_224:
		case STD_HASHTYPE_SHA2_256:
			rhash_sha256_final(&ctx->sha256, result);
			return true;
		case STD_HASHTYPE_SHA2_384:
		case STD_HASHTYPE_SHA2_512:
			rhash_sha512_final(&ctx->sha512, result);
			return true;
		case STD_HASHTYPE_SHA3_224:
		case STD_HASHTYPE_SHA3_256:
		case STD_HASHTYPE_SHA3_384:
		case STD_HASHTYPE_SHA3_512:
			rhash_sha3_final(&ctx->sha3, result);
			return true;
		case STD_HASHTYPE_BLK2_224:
			return (blake2b_final(&ctx->balke2, result, blk2_224_hash_size) == 0);
		case STD_HASHTYPE_BLK2_256:
			return (blake2b_final(&ctx->balke2, result, blk2_256_hash_size) == 0);
		case STD_HASHTYPE_BLK2_384:
			return (blake2b_final(&ctx->balke2, result, blk2_384_hash_size) == 0);
		case STD_HASHTYPE_BLK2_512:
			return (blake2b_final(&ctx->balke2, result, blk2_512_hash_size) == 0);
		default:
			return false;
	}
}

static void ConvertHashToHex(const size_t hashSize, const BYTE *const hashValue, TCHAR *const hashOut)
{
	size_t offset = 0;
	for(size_t i = 0; i < hashSize; i++)
	{
		hashOut[offset++] = LUT_BYTE2HEX[hashValue[i]][0];
		hashOut[offset++] = LUT_BYTE2HEX[hashValue[i]][1];
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

	hash_ctx ctx;
	if(!HashFunction_Init(hashType, &ctx))
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
		if(!HashFunction_Update(hashType, &ctx, buffer, nBytesRead))
		{
			CLOSE_HANDLE(h);
			return false;
		}
	}

	BYTE hashValue[128];
	if(HashFunction_Final(hashType, &ctx, hashValue))
	{
		ConvertHashToHex(hashSize, hashValue, hashOut);
		return true;
	}

	return false;
}

bool ComputeHash_FromText(const int hashType, const TCHAR *const textData, TCHAR *const hashOut, const size_t hashOutSize)
{
	const size_t hashSize = GetHashSize(hashType);
	if((hashSize == SIZE_MAX) || (hashSize >= (hashOutSize / 2)))
	{
		return false;
	}

	hash_ctx ctx;
	if(!HashFunction_Init(hashType, &ctx))
	{
		return false;
	}

#ifdef UNICODE
	const char *const textUtf8 = utf16_to_utf8(textData);
	if(textUtf8 == NULL)
	{
		return false;
	}
	if(!HashFunction_Update(hashType, &ctx, (BYTE*)textUtf8, strlen(textUtf8)))
	{
		delete [] textUtf8;
		return false;
	}
	delete [] textUtf8;
#else
	if(!HashFunction_Update(hashType, &ctx, (BYTE*)textData, strlen(textData)))
	{
		return false;
	}
#endif

	BYTE hashValue[128];
	if(HashFunction_Final(hashType, &ctx, hashValue))
	{
		ConvertHashToHex(hashSize, hashValue, hashOut);
		return true;
	}

	return false;
};
