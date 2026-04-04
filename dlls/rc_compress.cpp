// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 *    This file is part of RCBot.
 *
 *    RCBot by Paul Murphy adapted from botman's template 3.
 *
 *    RCBot is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    RCBot is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RCBot; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */
//////////////////////////////////////////////////
// RCBOT : Paul Murphy @ {cheeseh@rcbot.net}
//
// (http://www.rcbot.net)
//
// rc_compress.cpp
//
// Compression utilities using miniz for
// visibility tables and learned data files
//
//////////////////////////////////////////////////

#include "rc_compress.h"

#include <cstdlib>
#include <cstring>

#include "miniz.h"

bool RCZ_CompressToFile(const char* filename, const void* pData, const uint32_t dataSize)
{
std::FILE* fp = std::fopen(filename, "wb");
if (!fp)
return false;

const bool result = RCZ_CompressToFileHandle(fp, pData, dataSize);

std::fclose(fp);
return result;
}

bool RCZ_DecompressFromFile(const char* filename, void* pOutData, const uint32_t expectedSize)
{
std::FILE* fp = std::fopen(filename, "rb");
if (!fp)
return false;

const bool result = RCZ_DecompressFromFileHandle(fp, pOutData, expectedSize);

std::fclose(fp);
return result;
}

bool RCZ_IsCompressedFile(const char* filename)
{
std::FILE* fp = std::fopen(filename, "rb");
if (!fp)
return false;

RCZHeader header;
const size_t read = std::fread(&header, 1, sizeof(RCZHeader), fp);
std::fclose(fp);

if (read < sizeof(RCZHeader))
return false;

return std::memcmp(header.magic, RCZ_MAGIC, 4) == 0;
}

bool RCZ_CompressToFileHandle(std::FILE* fp, const void* pData, const uint32_t dataSize)
{
if (!fp || !pData || dataSize == 0)
return false;

// Allocate worst-case compressed buffer
mz_ulong compBound = mz_compressBound(static_cast<mz_ulong>(dataSize));
unsigned char* pCompressed = static_cast<unsigned char*>(std::malloc(compBound));
if (!pCompressed)
return false;

mz_ulong compSize = compBound;
const int status = mz_compress2(pCompressed, &compSize,
static_cast<const unsigned char*>(pData),
static_cast<mz_ulong>(dataSize), MZ_BEST_COMPRESSION);

if (status != MZ_OK)
{
std::free(pCompressed);
return false;
}

// Write header
RCZHeader header;
std::memcpy(header.magic, RCZ_MAGIC, 4);
header.version = RCZ_VERSION;
header.uncompressedSize = dataSize;
header.compressedSize = static_cast<uint32_t>(compSize);

if (std::fwrite(&header, sizeof(RCZHeader), 1, fp) != 1)
{
std::free(pCompressed);
return false;
}

// Write compressed data
if (std::fwrite(pCompressed, 1, compSize, fp) != compSize)
{
std::free(pCompressed);
return false;
}

std::free(pCompressed);
return true;
}

bool RCZ_DecompressFromFileHandle(std::FILE* fp, void* pOutData, const uint32_t expectedSize)
{
if (!fp || !pOutData || expectedSize == 0)
return false;

// Read header
RCZHeader header;
if (std::fread(&header, sizeof(RCZHeader), 1, fp) != 1)
return false;

// Validate magic
if (std::memcmp(header.magic, RCZ_MAGIC, 4) != 0)
return false;

// Validate version
if (header.version != RCZ_VERSION)
return false;

// Validate uncompressed size matches expected
if (header.uncompressedSize != expectedSize)
return false;

// Read compressed data
unsigned char* pCompressed = static_cast<unsigned char*>(std::malloc(header.compressedSize));
if (!pCompressed)
return false;

if (std::fread(pCompressed, 1, header.compressedSize, fp) != header.compressedSize)
{
std::free(pCompressed);
return false;
}

// Decompress
mz_ulong destLen = static_cast<mz_ulong>(expectedSize);
const int status = mz_uncompress(
static_cast<unsigned char*>(pOutData), &destLen,
pCompressed, static_cast<mz_ulong>(header.compressedSize));

std::free(pCompressed);

if (status != MZ_OK || destLen != expectedSize)
return false;

return true;
}

bool RCZ_DecompressFileToMemory(const char* filename, unsigned char** ppOutData, uint32_t* pOutSize)
{
if (!filename || !ppOutData || !pOutSize)
return false;

*ppOutData = nullptr;
*pOutSize = 0;

std::FILE* fp = std::fopen(filename, "rb");
if (!fp)
return false;

// Read header
RCZHeader header;
if (std::fread(&header, sizeof(RCZHeader), 1, fp) != 1)
{
std::fclose(fp);
return false;
}

// Validate magic
if (std::memcmp(header.magic, RCZ_MAGIC, 4) != 0)
{
std::fclose(fp);
return false;
}

if (header.version != RCZ_VERSION || header.uncompressedSize == 0 || header.compressedSize == 0)
{
std::fclose(fp);
return false;
}

// Read compressed data
unsigned char* pCompressed = static_cast<unsigned char*>(std::malloc(header.compressedSize));
if (!pCompressed)
{
std::fclose(fp);
return false;
}

if (std::fread(pCompressed, 1, header.compressedSize, fp) != header.compressedSize)
{
std::free(pCompressed);
std::fclose(fp);
return false;
}

std::fclose(fp);

// Allocate output buffer
unsigned char* pDecompressed = static_cast<unsigned char*>(std::malloc(header.uncompressedSize));
if (!pDecompressed)
{
std::free(pCompressed);
return false;
}

// Decompress
mz_ulong destLen = static_cast<mz_ulong>(header.uncompressedSize);
const int status = mz_uncompress(pDecompressed, &destLen,
pCompressed, static_cast<mz_ulong>(header.compressedSize));

std::free(pCompressed);

if (status != MZ_OK || destLen != header.uncompressedSize)
{
std::free(pDecompressed);
return false;
}

*ppOutData = pDecompressed;
*pOutSize = header.uncompressedSize;
return true;
}
