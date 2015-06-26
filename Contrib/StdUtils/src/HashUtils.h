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

#pragma once

#include "StdUtils.h"

#define STD_HASHTYPE_SHA3_224 1
#define STD_HASHTYPE_SHA3_256 2
#define STD_HASHTYPE_SHA3_384 3
#define STD_HASHTYPE_SHA3_512 4

bool ComputeHash_FromFile(const int hashType, const TCHAR *const fileName, TCHAR *const hashOut, const size_t hashOutSize);
bool ComputeHash_FromText(const int hashType, const TCHAR *const textData, TCHAR *const hashOut, const size_t hashOutSize);
