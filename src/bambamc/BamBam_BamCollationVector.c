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
#include <bambamc/BamBam_BamSingleAlignment_ToBam1.h>
#include <bambamc/BamBam_BamFlagBase.h>

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
	char const * bamheadertext
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
			(((BamBam_BamCollationHashEntry_GetFlags(vector->entries[i]) & BAMBAMC_FREAD1) && (BamBam_BamCollationHashEntry_GetFlags(vector->entries[i+1]) & BAMBAMC_FREAD2))
			||
			((BamBam_BamCollationHashEntry_GetFlags(vector->entries[i]) & BAMBAMC_FREAD2) && (BamBam_BamCollationHashEntry_GetFlags(vector->entries[i+1]) & BAMBAMC_FREAD1)))
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
		#if defined(BAMBAMC_BAMONLY)
		char * tmpfilename = BamBam_BamCollationTempFileGenerator_GetNextTempFileName(gen);
		
		#if 0
		BamBam_GzipWriter * gzipfile = 0;
		int status = -1;
		
		if ( ! tmpfilename )
			return -1;
		
		gzipfile = BamBam_GzipWriter_New(tmpfilename,1);
		
		if ( ! gzipfile )
			return -1;

		for ( i = 0; i < vector->fill; ++i )
		{
			int const r = BamBam_BamSingleAlignment_StoreAlignment(vector->entries[i]->entry,gzipfile);
			
			if ( r < 0 )
			{
				BamBam_GzipWriter_Delete(gzipfile,&status);		
				return -1;
			}
		}
			
		BamBam_GzipWriter_Delete(gzipfile,&status);
		
		if ( status < 0 )
			return -1;
		#else
		
		BamBam_BgzfCompressor * gzipfile = 0;
		int status = -1;

		if ( ! tmpfilename )
			return -1;
		
		gzipfile = BamBam_BgzfCompressor_New(tmpfilename,1);
		
		if ( ! gzipfile )
			return -1;

		for ( i = 0; i < vector->fill; ++i )
		{
			int const r = BamBam_BamSingleAlignment_StoreAlignmentBgzf(vector->entries[i]->entry,gzipfile);
			
			if ( r < 0 )
			{
				BamBam_BgzfCompressor_Delete(gzipfile);
				return -1;
			}
		}
		
		status = BamBam_BgzfCompressor_Terminate(gzipfile);
		
		if ( status < 0 )
		{
			BamBam_BgzfCompressor_Delete(gzipfile);
			return -1;	
		}
			
		BamBam_BgzfCompressor_Delete(gzipfile);
		
		#endif
		
		#else
		char * tmpfilename = BamBam_BamCollationTempFileGenerator_GetNextTempFileName(gen);
		bamFile bamfile;
		bam_header_t * bamheader = 0;
		#if defined(BAMBAMC_BAMONLY)
		bam1_t * talgn = 0;
		#endif
		
		if ( ! tmpfilename )
			return -1;

		#if defined(BAMBAMC_BAMONLY)
		talgn = bam_init1();
		if ( ! talgn )
			return -1;
		#endif
		
		bamfile = bam_open(tmpfilename, "w1");
		
		if ( ! bamfile )
		{	
			#if defined(BAMBAMC_BAMONLY)
			bam_destroy1(talgn);
			#endif
			return -1;
		}

		bamheader = bam_header_init();
		
		if ( ! bamheader )
		{
			#if defined(BAMBAMC_BAMONLY)
			bam_destroy1(talgn);
			#endif
			bam_close(bamfile);
			return -1;
		}
		
		bamheader->text = strdup(bamheadertext);
		
		if ( ! bamheader->text )
		{
			#if defined(BAMBAMC_BAMONLY)
			bam_destroy1(talgn);
			#endif
			bam_header_destroy(bamheader);
			bam_close(bamfile);
			return -1;
		}
		
		bamheader->l_text = strlen(bamheader->text);
		sam_header_parse(bamheader);

		bam_header_write(bamfile,bamheader);
		
		#if defined(BAMBAMC_BAMONLY)
		for ( i = 0; i < vector->fill; ++i )
		{
			int const r = BamBam_BamSingleAlignment_ToBam1(vector->entries[i]->entry, talgn);
			if ( r < 0 )
			{
				#if defined(BAMBAMC_BAMONLY)
				bam_destroy1(talgn);
				#endif
				bam_header_destroy(bamheader);
				bam_close(bamfile);		
				return -1;
			}
			bam_write1(bamfile,talgn);
		}
		#else
		for ( i = 0; i < vector->fill; ++i )
			bam_write1(bamfile,vector->entries[i]->entry);
		#endif

		#if defined(BAMBAMC_BAMONLY)
		bam_destroy1(talgn);
		#endif
		bam_header_destroy(bamheader);
		bam_close(bamfile);				
		#endif
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
