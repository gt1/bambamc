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
#if ! defined(BAMBAMC_GZIPWRITER_H)
#define BAMBAMC_GZIPWRITER_H

#include <zlib.h>
#include <stdint.h>

typedef struct _BamBam_GzipWriter
{
	gzFile file;
} BamBam_GzipWriter;

extern BamBam_GzipWriter * BamBam_GzipWriter_New_Filename(char const * filename, int level);
extern BamBam_GzipWriter * BamBam_GzipWriter_New_Fd(int fd, int level);
extern BamBam_GzipWriter * BamBam_GzipWriter_New(char const * filename, int level);
extern BamBam_GzipWriter * BamBam_GzipWriter_Delete(BamBam_GzipWriter * object, int * state);
extern int BamBam_GzipWriter_Putc(BamBam_GzipWriter * object, int c);
extern int BamBam_GzipWriter_PutInt32(BamBam_GzipWriter * object, int32_t const v);
extern int BamBam_GzipWriter_PutUInt32(BamBam_GzipWriter * object, uint32_t const v);
extern int BamBam_GzipWriter_Write(BamBam_GzipWriter * object, char const * c, int l);
#endif
