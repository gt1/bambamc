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

#if ! defined(BAMBAM_BAMCOLLATOR_H)
#define BAMBAM_BAMCOLLATOR_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_BamCollationHash.h>
#include <bambamc/BamBam_BamCollationVector.h>
#include <bambamc/BamBam_BamCollationOutputVector.h>
#include <bambamc/BamBam_MergeHeapEntry.h>
#include <bambamc/BamBam_BamHeader.h>
#include <bambamc/BamBam_BamHeaderInfo.h>

#include <bambamc/BamBam_Config.h>

#if defined(HAVE_SAM_H)
#include <sam.h>
#endif
#if defined(HAVE_SAMTOOLS_SAM_H)
#include <samtools/sam.h>
#endif

typedef struct _BamBam_BamCollator
{
	int state;
	int tmpdirstate;
	char * tempdirname;
	char * tempprefix;
	BamBam_BamCollationHash * hash;
	BamBam_BamCollationVector * vector;
	BamBam_BamCollationTempFileGenerator * gen;
	BamBam_BamCollationOutputVector * outputvector;
	samfile_t * bamfile;
	bam_header_t * bamheader;
	/* header text */
	char * bamheadertext;
	char * filteredbamheadertext;
	char * headerline;
	char * vn;
	char * so;
	BamBam_BamHeaderInfo * parsedheaderinfo;
	BamBam_BamHeaderInfo * headerinfo;
	bam1_t * alignment;
	
	uint64_t nummergefiles;
	samfile_t ** mergefiles;
	BamBam_MergeHeapEntry * mergeheap;
	uint64_t mergeheapfill;
	
	char * bufferA;
	unsigned int bufferALength;
	char * bufferB;
	unsigned int bufferBLength;
	
	int keepOrphans;
	
	BamBam_BamCollationHashEntry * entryA;
	BamBam_BamCollationHashEntry * entryB;
} BamBam_BamCollator;

extern void BamBam_BamCollator_MergeHeapEntrySwap(BamBam_MergeHeapEntry * A, BamBam_MergeHeapEntry * B);
extern void BamBam_BamCollator_MergeHeapPrint(BamBam_BamCollator * collator);
extern void BamBam_BamCollator_MergeHeapCheck(BamBam_BamCollator * collator);
extern BamBam_MergeHeapEntry * BamBam_BamCollator_MergeHeapMinimum(BamBam_BamCollator * collator) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollator_MergeHeapDeleteMinimum(BamBam_BamCollator * collator);
extern void BamBam_BamCollator_MergeHeapInsert(BamBam_BamCollator * collator, BamBam_BamCollationHashEntry * hashentry, uint64_t fileid);
extern BamBam_BamCollationHashEntry * BamBam_BamCollator_GetNextRead(BamBam_BamCollator * collator) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BamCollator * BamBam_BamCollator_Delete(BamBam_BamCollator * collator);
extern BamBam_BamCollator * BamBam_BamCollator_New(
	char const * tempdirname,
	unsigned int const hashsizelog,
	unsigned int const vectorsizelog,
	char const * inputformat,
	char const * inputfilename
) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamCollator_Get(
	BamBam_BamCollator * collator, 
	BamBam_BamCollationHashEntry ** entryA, 
	BamBam_BamCollationHashEntry ** entryB) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
