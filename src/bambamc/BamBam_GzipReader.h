/**
    bambamc
    Copyright (C) 2009-2013 German Tischler
    Copyright (C) 2011-2013 Genome Research Limited

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#if ! defined(BAMBAMC_GZIPREADER_H)
#define BAMBAMC_GZIPREADER_H

#include <zlib.h>
#include <stdint.h>

typedef struct _BamBam_GzipReader
{
	gzFile file;
} BamBam_GzipReader;

extern BamBam_GzipReader * BamBam_GzipReader_New_Filename(char const * filename);
extern BamBam_GzipReader * BamBam_GzipReader_New_Fd(int fd);
extern BamBam_GzipReader * BamBam_GzipReader_New(char const * filename);
extern BamBam_GzipReader * BamBam_GzipReader_Delete(BamBam_GzipReader * object);
extern int BamBam_GzipReader_Peek(BamBam_GzipReader * object);
extern int BamBam_GzipReader_Getc(BamBam_GzipReader * object);
extern int BamBam_GzipReader_GetInt32(BamBam_GzipReader * object, int32_t * v);
extern int BamBam_GzipReader_GetUInt32(BamBam_GzipReader * object, uint32_t * v);
extern int BamBam_GzipReader_Read(BamBam_GzipReader * object, char * c, int l);
#endif
