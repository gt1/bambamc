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
	/* length of orignal text */
	int32_t l_text;
	/* original text */
	char * text;
	/* number of chromosomes/references */
	int32_t n_ref;
	/* linked list of chromosomes */
	BamBam_List * chromosomes;
	/* chromosomes in numerical id order */
	BamBam_Chromosome ** chromosomevec;
	/* chromosomes in name sorted order */
	BamBam_Chromosome ** sortedchromosomevec;
	/* single header lines */
	char ** headerlines;
	/* HD header line */
	char const * hdline;
	/* sort order (coordinate or queryname) */
	char * sortorder;
	/* bam file version */
	char * version;
	/* processed header text */
	char * headertext;
} BamBam_BamFileHeader;

extern BamBam_BamFileHeader * BamBam_BamFileHeader_New_SAM(FILE * reader);
extern BamBam_BamFileHeader * BamBam_BamFileHeader_New_BAM(BamBam_GzipReader * reader);
extern BamBam_BamFileHeader * BamBam_BamFileHeader_Delete(BamBam_BamFileHeader * object);
extern BamBam_Chromosome const * BamBam_BamFileHeader_FindChromosomeByName(BamBam_BamFileHeader const * header, char const * name);
extern int BamBam_BamFileHeader_FindChromosomeIdByName(BamBam_BamFileHeader const * header, char const * name);
#endif
