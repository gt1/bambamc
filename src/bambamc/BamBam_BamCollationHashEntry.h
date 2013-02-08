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

#if ! defined(BAMBAM_BAMCOLLATIONHASHENTRY_H)
#define BAMBAM_BAMCOLLATIONHASHENTRY_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_BamCollationTempFileGenerator.h>
#include <bambamc/BamBam_Hash.h>

#include <bambamc/BamBam_Config.h>

#if defined(HAVE_SAM_H)
#include <sam.h>
#endif
#if defined(HAVE_SAMTOOLS_SAM_H)
#include <samtools/sam.h>
#endif

typedef struct _BamBam_BamCollationHashEntry
{
	bam1_t * entry;
	char * qname;
	unsigned int qnamelen;
	uint32_t hashvalue;
} BamBam_BamCollationHashEntry;

extern int BamBam_BamHashEntry_Compare(BamBam_BamCollationHashEntry const * A, BamBam_BamCollationHashEntry const * B) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamHashEntry_CompareVerbose(BamBam_BamCollationHashEntry const * A, BamBam_BamCollationHashEntry const * B) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamHashEntry_CompareVoidPtr(const void * VA, const void * VB) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollationHashEntry_Delete(BamBam_BamCollationHashEntry * hashentry);
extern uint32_t BamBam_BamCollationHashEntry_GetFlags(BamBam_BamCollationHashEntry const * hashentry) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BamCollationHashEntry * BamBam_BamCollationHashEntry_NewDup(bam1_t * alignment) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
