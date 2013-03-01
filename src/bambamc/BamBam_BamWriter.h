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

#if ! defined(BAMBAM_BAMWRITER_H)
#define BAMBAM_BAMWRITER_H

#include <bambamc/BamBam_BamAlignmentPut.h>
#include <bambamc/BamBam_BamHeaderInfo.h>

typedef struct _BamBam_BamWriter
{
	BamBam_AlignmentPut * aput;
	BamBam_BgzfCompressor * bgzf;
} BamBam_BamWriter;

extern BamBam_BamWriter * BamBam_BamWriter_Delete(BamBam_BamWriter * writer, int * termstatus);
extern BamBam_BamWriter * BamBam_BamWriter_New(
	BamBam_BamHeaderInfo * info,
	char const * filename,
	int compressionLevel) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamWriter_PutAlignment(
	BamBam_BamWriter * writer,
	/* flags */
	int32_t const flags,
	/* target (chromosome) id */
	int32_t const tid,
	/* position on chromosome (0 based) */
	uint64_t const rpos,
	/* mate target id */
	int32_t const mtid,
	/* position of mate on mate target id */
	uint64_t const rmpos,
	/* sequence name */
	char const * name,
	/* query sequence (read) */
	char const * query,
	/* quality string */
	char const * qual,
	/* cigar operations */
	char const * cigar,
	/* mapping quality */
	int32_t const rqual,
	/* insert size */
	int32_t const isize
	) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamWriter_PutAuxNumber(BamBam_BamWriter * writer, char const * tag, char const type, void const * rvalue) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamWriter_Commit(BamBam_BamWriter * writer);
#endif
