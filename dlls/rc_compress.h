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
// rc_compress.h
//
// Compression utilities using miniz for
// visibility tables and learned data files
//
//////////////////////////////////////////////////

#ifndef RC_COMPRESS_H
#define RC_COMPRESS_H

#include <cstdio>
#include <cstdint>

// Compressed file header magic: "RCZ\0"
constexpr char RCZ_MAGIC[4] = { 'R', 'C', 'Z', '\0' };
constexpr uint32_t RCZ_VERSION = 1;

#pragma pack(push, 1)
struct RCZHeader
{
char magic[4];
uint32_t version;
uint32_t uncompressedSize;
uint32_t compressedSize;
};
#pragma pack(pop)

// Compress a memory buffer and write it to a file.
// Returns true on success, false on failure.
bool RCZ_CompressToFile(const char* filename, const void* pData, uint32_t dataSize);

// Read a compressed file into a memory buffer.
// pOutData must point to a buffer of at least expectedSize bytes.
// Returns true on success, false on failure (including if the file is not compressed).
bool RCZ_DecompressFromFile(const char* filename, void* pOutData, uint32_t expectedSize);

// Check if a file has the RCZ compressed header.
bool RCZ_IsCompressedFile(const char* filename);

// Compress a memory buffer and write it to an already-open FILE.
// Returns true on success, false on failure.
bool RCZ_CompressToFileHandle(std::FILE* fp, const void* pData, uint32_t dataSize);

// Read a compressed block from an already-open FILE into a memory buffer.
// pOutData must point to a buffer of at least expectedSize bytes.
// Returns true on success, false on failure.
bool RCZ_DecompressFromFileHandle(std::FILE* fp, void* pOutData, uint32_t expectedSize);

// Decompress a compressed file entirely into a newly-allocated memory buffer.
// On success, *ppOutData is set to the allocated buffer and *pOutSize is set to its size.
// The caller must free *ppOutData with std::free() when done.
// Returns true on success, false on failure.
bool RCZ_DecompressFileToMemory(const char* filename, unsigned char** ppOutData, uint32_t* pOutSize);

#endif // RC_COMPRESS_H
