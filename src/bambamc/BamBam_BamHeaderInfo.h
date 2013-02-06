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

#if ! defined(BAMBAM_BAMHEADERINFO_H)
#define BAMBAM_BAMHEADERINFO_H

#include <bambamc/BamBam_List.h>
#include <bambamc/BamBam_CharBuffer.h>
#include <bambamc/BamBam_Chromosome.h>

typedef struct _BamBam_BamHeaderInfo
{
	char * version;
	char * sortorder;
	char * plaintext;
	BamBam_List * chrlist;
	BamBam_CharBuffer * cb;
} BamBam_BamHeaderInfo;

extern int BamBam_BamHeaderInfo_ProduceHeaderText(BamBam_BamHeaderInfo * info) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BamHeaderInfo * BamBam_BamHeaderInfo_Delete(BamBam_BamHeaderInfo * info);
extern BamBam_BamHeaderInfo * BamBam_BamHeaderInfo_New(char const * version, char const * sortorder, char const * plaintext) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamHeaderInfo_AddChromosome(BamBam_BamHeaderInfo * info, char const * name, uint64_t len) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
