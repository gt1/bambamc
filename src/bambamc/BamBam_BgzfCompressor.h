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
#if ! defined(BAMBAMC_BGZFCOMPRESSOR_H)
#define BAMBAMC_BGZFCOMPRESSOR_H

#include <bambamc/BamBam_Unused.h>
#include <stdio.h>
#include <zlib.h>
#include <stdint.h>

/* gzip/zlib header flags */
enum bambamc_gzipheader_flags {
	BAMBAMC_LZ_FTEXT = (1u << 0),
	BAMBAMC_LZ_FHCRC = (1u << 1),
	BAMBAMC_LZ_FEXTRA = (1u << 2),
	BAMBAMC_LZ_FNAME = (1u << 3),
	BAMBAMC_LZ_FCOMMENT = (1u << 4),
	BAMBAMC_LZ_FRES0 = (1u<<5),
	BAMBAMC_LZ_FRES1 = (1u<<6),
	BAMBAMC_LZ_FRES2 = (1u<<7),
	BAMBAMC_LZ_FRES = ((1u<<5)|(1u<<6)|(1u<<7))
};

/* gzip header id */
#define BAMBAMC_LZ_ID1 0x1F
#define BAMBAMC_LZ_ID2 0x8B
/* compression method */
#define BAMBAMC_LZ_CM 8 /* compression method: deflate */
/* unknown operation system id */
#define BAMBAMC_LZ_OS_UNKNOWN 255
/* maximum buffer size */
#define BAMBAMC_LZ_MAXBUFSIZE (64*1024)

typedef struct _BamBam_BgzfCompressor
{
	FILE * file;
	FILE * closefile;
	
	int level;

	Bytef * inbuffer;
	uint32_t inbufferfill;
	Bytef * outbuffer;
} BamBam_BgzfCompressor;

extern BamBam_BgzfCompressor * BamBam_BgzfCompressor_New(char const * filename, int const level) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BgzfCompressor * BamBam_BgzfCompressor_NewFilename(char const * filename, int const level) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BgzfCompressor * BamBam_BgzfCompressor_NewFP(FILE * rfile, int const level) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BgzfCompressor * BamBam_BgzfCompressor_Delete(BamBam_BgzfCompressor * object);
extern int BamBam_BgzfCompressor_Flush(BamBam_BgzfCompressor * object) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BgzfCompressor_Write(BamBam_BgzfCompressor * object, uint8_t const * data, uint32_t len) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BgzfCompressor_PutInt32(BamBam_BgzfCompressor * object, int32_t const v) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BgzfCompressor_PutStringZ(BamBam_BgzfCompressor * object, char const * const c) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BgzfCompressor_PutLenStringZ(BamBam_BgzfCompressor * object, char const * const c) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BgzfCompressor_Terminate(BamBam_BgzfCompressor * object) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
