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
#if ! defined(BAMBAM_BAMALIGNMENTPUT_H)
#define BAMBAM_BAMALIGNMENTPUT_H

#include <bambamc/BamBam_CharBuffer.h>

#include <assert.h>
#include <ctype.h>

#include <bambamc/BamBam_Config.h>

#if defined(HAVE_SAM_H)
#include <sam.h>
#endif
#if defined(HAVE_SAMTOOLS_SAM_H)
#include <samtools/sam.h>
#endif

typedef struct _BamBam_AlignmentPut
{
	BamBam_CharBuffer * charbuffer;
	bam1_t * alignment;
} BamBam_AlignmentPut;

extern BamBam_AlignmentPut * BamBam_AlignmentPut_New() BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_AlignmentPut * BamBam_AlignmentPut_Delete(BamBam_AlignmentPut * aput);

extern int BamBam_CharBuffer_PutAlignment(
	BamBam_AlignmentPut * aput,
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
extern int BamBam_CharBuffer_PutAuxNumber(BamBam_AlignmentPut * aput, char const * tag, char const type, void const * rvalue) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
