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

#include <bambamc/BamBam_BamCollationVector.h>

BamBam_BamCollationVector * BamBam_BamCollationVector_New(unsigned int sizelog)
{
	BamBam_BamCollationVector * vector = 0;
	uint64_t i;
	
	vector = (BamBam_BamCollationVector *)malloc(sizeof(BamBam_BamCollationVector));

	if ( ! vector )
		return 0;
		
	vector->entries = 0;
	vector->tablesize = 1ull << sizelog;
	vector->fill = 0;
	
	vector->entries = (BamBam_BamCollationHashEntry **)malloc(vector->tablesize*(sizeof(BamBam_BamCollationHashEntry *)));
	
	if ( ! vector->entries )
	{
		free(vector);
		return 0;
	}
	
	for ( i = 0; i < vector->tablesize; ++i )
		vector->entries[i] = 0;
	
	return vector;
}

int BamBam_BamCollationVector_PushBack(BamBam_BamCollationVector * vector, BamBam_BamCollationHashEntry * entry)
{
	vector->entries[vector->fill++] = entry;
	return (vector->fill == vector->tablesize);
}

void BamBam_BamCollationVector_Erase(BamBam_BamCollationVector * vector)
{
	uint64_t i;
	for ( i = 0; i < vector->fill; ++i )
		BamBam_BamCollationHashEntry_Delete(vector->entries[i]);
	vector->fill = 0;
}

int BamBam_BamCollationVector_Sort(
	BamBam_BamCollationVector * vector,
	BamBam_BamCollationOutputVector * outputvector,
	BamBam_BamCollationTempFileGenerator * gen,
	bam_header_t * bamheader
)
{
	uint64_t i, j;

	qsort ( 
		&(vector->entries[0]), 
		vector->fill, 
		sizeof(BamBam_BamCollationHashEntry *), 
		BamBam_BamHashEntry_CompareVoidPtr
	);

	j = 0;
	for ( i = 0; i < vector->fill; )
	{	
		if ( 
			i+1 < vector->fill &&
			strcmp(vector->entries[i]->qname,vector->entries[i+1]->qname) == 0 &&
			(((BamBam_BamCollationHashEntry_GetFlags(vector->entries[i]) & BAM_FREAD1) && (BamBam_BamCollationHashEntry_GetFlags(vector->entries[i+1]) & BAM_FREAD2))
			||
			((BamBam_BamCollationHashEntry_GetFlags(vector->entries[i]) & BAM_FREAD2) && (BamBam_BamCollationHashEntry_GetFlags(vector->entries[i+1]) & BAM_FREAD1)))
		)
		{
			/* fprintf(stderr,"Found pair %s %s\n", vector->entries[i]->qname, vector->entries[i+1]->qname); */
			BamBam_BamCollationOutputVector_PushBack(outputvector,vector->entries[i  ]);
			BamBam_BamCollationOutputVector_PushBack(outputvector,vector->entries[i+1]);
									
			i += 2;
		}
		else
		{
			vector->entries[j++] = vector->entries[i];
			i++;
		}
	}

	vector->fill = j;
	
	if ( vector->fill )
	{
		char * tmpfilename = BamBam_BamCollationTempFileGenerator_GetNextTempFileName(gen);
		bamFile bamfile;
		
		if ( ! tmpfilename )
			return -1;
		
		bamfile = bam_open(tmpfilename, "w1");
		
		if ( ! bamfile )
			return -1;

		bam_header_write(bamfile,bamheader);
		
		for ( i = 0; i < vector->fill; ++i )
			bam_write1(bamfile,vector->entries[i]->entry);

		bam_close(bamfile);
	}
	
	return 0;
}

void BamBam_BamCollationVector_Delete(BamBam_BamCollationVector * vector)
{
	if ( vector )
	{
		uint64_t i;
		for ( i = 0; i < vector->fill; ++i )
			if ( vector->entries[i] )
				BamBam_BamCollationHashEntry_Delete(vector->entries[i]);
	
		free(vector->entries);
		free(vector);
	}
}
