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
#if ! defined(BAMBAM_BAMFILEHEADER_H)
#define BAMBAM_BAMFILEHEADER_H

#include <stdint.h>
#include <bambamc/BamBam_List.h>
#include <bambamc/BamBam_Chromosome.h>
#include <bambamc/BamBam_GzipReader.h>

typedef struct _BamBam_BamFileHeader
{
	int32_t l_text;
	char * text;
	int32_t n_ref;
	BamBam_List * chromosomes;
	BamBam_Chromosome ** chromosomevec;
	BamBam_Chromosome ** sortedchromosomevec;
	char ** headerlines;
	char const * hdline;
	char * sortorder;
	char * version;
	char * headertext;
} BamBam_BamFileHeader;

extern BamBam_BamFileHeader * BamBam_BamFileHeader_New_SAM(FILE * reader);
extern BamBam_BamFileHeader * BamBam_BamFileHeader_New_BAM(BamBam_GzipReader * reader);
extern BamBam_BamFileHeader * BamBam_BamFileHeader_Delete(BamBam_BamFileHeader * object);
#endif
