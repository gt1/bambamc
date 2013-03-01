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

#include <bambamc/BamBam_BamCollator.h>
#include <bambamc/BamBam_CharBuffer.h>
#include <bambamc/BamBam_LineParsing.h>
#include <assert.h>
#include <ctype.h>

void BamBam_BamCollator_MergeHeapEntrySwap(BamBam_MergeHeapEntry * A, BamBam_MergeHeapEntry * B)
{
	{
		uint64_t t = B->fileid;
		B->fileid = A->fileid;
		A->fileid = t;
	}
	{
		BamBam_BamCollationHashEntry * t = B->hashentry;
		B->hashentry = A->hashentry;
		A->hashentry = t;
	}
}

void BamBam_BamCollator_MergeHeapPrint(BamBam_BamCollator * collator)
{
	uint64_t p;
	for ( p = 0; p < collator->mergeheapfill; ++p )
		fprintf(stderr,"HEAP[%llu]=%s\n",(unsigned long long)p,collator->mergeheap[p].hashentry->qname);
}

void BamBam_BamCollator_MergeHeapCheck(BamBam_BamCollator * collator)
{
	uint64_t p;
	for ( p = 1; p < collator->mergeheapfill; ++p )
	{
		uint64_t const parent = (p-1)/2;
		BamBam_MergeHeapEntry * Pp = &(collator->mergeheap[p]);
		BamBam_MergeHeapEntry * Pparent = &(collator->mergeheap[parent]);
		assert ( BamBam_BamHashEntry_Compare( Pparent->hashentry, Pp->hashentry ) <= 0 );
	}
}

static inline uint64_t BamBam_BamCollator_MergeHeapMinimum2(BamBam_BamCollator * collator, uint64_t p0, uint64_t p1)
{
	BamBam_MergeHeapEntry * Pp0 = &(collator->mergeheap[p0]);
	BamBam_MergeHeapEntry * Pp1 = &(collator->mergeheap[p1]);
	
	if ( BamBam_BamHashEntry_Compare( Pp0->hashentry, Pp1->hashentry ) <= 0 )
		return p0;
	else
		return p1;
}

static inline uint64_t BamBam_BamCollator_MergeHeapMinimum3(BamBam_BamCollator * collator, uint64_t p0, uint64_t p1, uint64_t p2)
{
	return BamBam_BamCollator_MergeHeapMinimum2(collator,p0,BamBam_BamCollator_MergeHeapMinimum2(collator,p1,p2));
}

BamBam_MergeHeapEntry * BamBam_BamCollator_MergeHeapMinimum(BamBam_BamCollator * collator)
{
	assert ( collator->mergeheapfill );	
	return &(collator->mergeheap[0]);
}

void BamBam_BamCollator_MergeHeapDeleteMinimum(BamBam_BamCollator * collator)
{
	BamBam_MergeHeapEntry * first;
	BamBam_MergeHeapEntry * last;
	uint64_t p = 0;

	assert ( collator->mergeheapfill );	

	/* swap first and last element, then delete the former minimum */
	first = &(collator->mergeheap[0]);
	last  = &(collator->mergeheap[collator->mergeheapfill-1]);
	
	BamBam_BamCollator_MergeHeapEntrySwap(first,last);
	BamBam_BamCollationHashEntry_Delete(last->hashentry);
	collator->mergeheapfill -= 1;
	
	/* push former last element down the heap until the heap property holds */
	while ( 2*p+1 < collator->mergeheapfill )
	{
		/* both children exist */
		if ( 2*p+2 < collator->mergeheapfill )
		{
			/* find minimum of this node and both children */
			uint64_t const mini = BamBam_BamCollator_MergeHeapMinimum3(collator,p,2*p+1,2*p+2);
			
			/* if minimum is this node, then heap property holds */
			if ( mini == p )
			{
				break;
			}
			/* otherwise swap minimum with this node and recurse into corresponding subtree */
			else
			{
				BamBam_MergeHeapEntry * Pp = &(collator->mergeheap[p]);
				BamBam_MergeHeapEntry * Pc = &(collator->mergeheap[mini]);
				BamBam_BamCollator_MergeHeapEntrySwap(Pp,Pc);
				p = mini;
			}	
		}
		/* only left child exists */
		else
		{
			/* if minimum is this node, then heap property holds */
			if ( BamBam_BamCollator_MergeHeapMinimum2(collator,p,2*p+1) == p )
			{
				break;
			}
			/* otherwise swap and recurse */
			else
			{
				BamBam_MergeHeapEntry * Pp = &(collator->mergeheap[p]);
				BamBam_MergeHeapEntry * Pl = &(collator->mergeheap[2*p+1]);
				BamBam_BamCollator_MergeHeapEntrySwap(Pp,Pl);
				p = 2*p+1;
			}
		}
	}
}

void BamBam_BamCollator_MergeHeapInsert(BamBam_BamCollator * collator, BamBam_BamCollationHashEntry * hashentry, uint64_t fileid)
{
	uint64_t p;
	int swapped = 1;
	
	/* insert new element at the end of the heap */
	p = collator->mergeheapfill++;
	assert ( collator->mergeheapfill <= collator->nummergefiles );
	collator->mergeheap [ p ] . hashentry = hashentry;
	collator->mergeheap [ p ] . fileid = fileid;
	
	/* move it up */
	while ( p && swapped )
	{
		uint64_t parent = (p-1)/2;
		BamBam_MergeHeapEntry * Pp = &(collator->mergeheap[p]);
		BamBam_MergeHeapEntry * Pparent = &(collator->mergeheap[parent]);
		
		if ( BamBam_BamHashEntry_Compare( Pp->hashentry, Pparent->hashentry ) < 0 )
		{
			BamBam_BamCollator_MergeHeapEntrySwap(Pp,Pparent);
		}
		else
		{
			swapped = 0;
		}
		
		p = parent;
	}
}

#include <bambamc/BamBam_CollatorState.h>

BamBam_BamCollationHashEntry * BamBam_BamCollator_GetNextRead(BamBam_BamCollator * collator)
{
	while ( (collator->state==BAMBAM_COLLATE_READING_INPUT) && !collator->outputvector->outputvectorfill )
	{
		#if defined(BAMBAMC_BAMONLY)
		if ( (collator->alignment = BamBam_SamBamFileDecoder_DecodeAlignment(collator->decoder)) )
		#else
		if ( samread(collator->bamfile,collator->alignment) >= 0 )
		#endif
		{
			BamBam_BamCollationHashEntry * nhashentry = 0;
			BamBam_BamCollationHashEntry * ohashentry = 0;
			
			nhashentry = BamBam_BamCollationHashEntry_NewDup(collator->alignment);
			
			if ( ! nhashentry )
			{
				collator->state = BAMBAM_COLLATE_FAILED;
			}
			else
			{	
				uint32_t nhashentryflags;
				nhashentryflags = BamBam_BamCollationHashEntry_GetFlags(nhashentry);
				
				/* single end? */
				if ( !(nhashentryflags & BAMBAMC_FPAIRED) )
				{
					BamBam_BamCollationOutputVector_PushBack(collator->outputvector,nhashentry);
				}
				/* pair */
				else
				{
					ohashentry = collator->hash->entries[ nhashentry->hashvalue & collator->hash->tablemask ];

					/* fprintf(stderr,"Got name %s\n", hashentry->qname); */
					
					/* hash place is empty */
					if ( ! ohashentry )
					{
						collator->hash->entries[ nhashentry->hashvalue & collator->hash->tablemask ] = nhashentry;
					}
					else
					{
						uint32_t ohashentryflags = BamBam_BamCollationHashEntry_GetFlags(ohashentry);
						
						if ( 
							ohashentry->hashvalue == nhashentry->hashvalue
							&&
							! strcmp(ohashentry->qname,nhashentry->qname)
							&& 
							(
								((nhashentryflags & BAMBAMC_FREAD1) && (ohashentryflags & BAMBAMC_FREAD2))
								||
								((nhashentryflags & BAMBAMC_FREAD2) && (ohashentryflags & BAMBAMC_FREAD1))
							)
						)
						{
							/* found pair */
							collator->hash->entries[ nhashentry->hashvalue & collator->hash->tablemask ] = 0;
							if ( (ohashentryflags & BAMBAMC_FREAD1) != 0 )
							{
								BamBam_BamCollationOutputVector_PushBack(collator->outputvector,ohashentry);
								BamBam_BamCollationOutputVector_PushBack(collator->outputvector,nhashentry);
							}
							else
							{
								BamBam_BamCollationOutputVector_PushBack(collator->outputvector,nhashentry);
								BamBam_BamCollationOutputVector_PushBack(collator->outputvector,ohashentry);						
							}
						}
						else
						{
							int vecfull;
							/* not pair, push previous entry out */
							vecfull = BamBam_BamCollationVector_PushBack(collator->vector,ohashentry);
							
							if ( vecfull )
							{
								int sortok;
								
								sortok = BamBam_BamCollationVector_Sort(collator->vector,collator->outputvector,collator->gen,
									(char const *)(collator->headerinfo->cb->buffer));
								
								if ( sortok < 0 )
								{
									collator->state = BAMBAM_COLLATE_FAILED;
								}
								
								BamBam_BamCollationVector_Erase(collator->vector);
							}

							collator->hash->entries[ nhashentry->hashvalue & collator->hash->tablemask ] = nhashentry;
						}
					}
				}
			}
		}
		/* no more alignments in input file */
		else
		{
			/* handle alignments left in hash table */
			uint64_t i = 0;
			int sortok;
			
			for ( i = 0; collator->state != BAMBAM_COLLATE_FAILED && i < collator->hash->tablesize; ++i )
				if ( collator->hash->entries[i] )
				{
					int vecfull;
					int sortok;
					BamBam_BamCollationHashEntry * hashentry = collator->hash->entries[i];
					collator->hash->entries[i] = 0;

					/* push entry out */
					vecfull = BamBam_BamCollationVector_PushBack(collator->vector,hashentry);
				
					if ( vecfull )
					{
						/* fprintf(stderr,"erasing full vector.\n"); */
						sortok = BamBam_BamCollationVector_Sort(collator->vector,collator->outputvector,collator->gen,
							(char const *)(collator->headerinfo->cb->buffer));
				
						if ( sortok < 0 )
							collator->state = BAMBAM_COLLATE_FAILED;
					
						BamBam_BamCollationVector_Erase(collator->vector);
					}
				}

			sortok = BamBam_BamCollationVector_Sort(collator->vector,collator->outputvector,collator->gen,
				(char const *)(collator->headerinfo->cb->buffer));
			if ( sortok < 0 )
				collator->state = BAMBAM_COLLATE_FAILED;
			BamBam_BamCollationVector_Erase(collator->vector);
		
			collator->state = BAMBAM_COLLATE_MERGING;
		}
	}
	while ( (collator->state==BAMBAM_COLLATE_MERGING) && !collator->outputvector->outputvectorfill )
	{
		/* tempfiles not yet open, open them */
		if ( ! collator->mergefiles )
		{
			uint64_t numtempfiles;
			BamBam_ListNode * node = 0;
			uint64_t k = 0;
			
			numtempfiles = BamBam_List_Size(collator->gen->tempfilenames);			
			#if defined(BAMBAMC_BAMONLY)
			collator->mergefiles = (BamBam_GzipFileDecoder **)malloc(numtempfiles * sizeof(BamBam_GzipFileDecoder *));
			#else
			collator->mergefiles = (samfile_t **)malloc(numtempfiles * sizeof(samfile_t *));
			#endif
			
			if ( ! collator->mergefiles )
				collator->state = BAMBAM_COLLATE_FAILED;
			else
			{
				collator->nummergefiles = numtempfiles;
				for ( k = 0; k < numtempfiles; ++k )
					collator->mergefiles[k] = 0;
				
				node = collator->gen->tempfilenames->first;
			
				k = 0;
				while ( node )
				{
					char const * filename = (char const *)(node->entry);
					
					#if defined(BAMBAMC_BAMONLY)
					collator->mergefiles[k] = BamBam_GzipFileDecoder_New(filename);
					#else
					collator->mergefiles[k] = samopen(filename,"rb",0);
					#endif
				
					if ( ! collator->mergefiles[k] )
						collator->state = BAMBAM_COLLATE_FAILED;
				
					node = node->next;
					k++;
				}
				
				/* if we managed to open all temporary files, then allocate heap */
				if ( collator->state != BAMBAM_COLLATE_FAILED )
				{
					collator->mergeheap = (BamBam_MergeHeapEntry *)malloc(numtempfiles * sizeof(BamBam_MergeHeapEntry));
					
					if ( ! collator->mergeheap )
						collator->state = BAMBAM_COLLATE_FAILED;						
					else
					{
						for ( k = 0; k < numtempfiles; ++k )
						{
							collator->mergeheap[k].hashentry = 0;
							collator->mergeheap[k].fileid = numtempfiles;
						}
						collator->mergeheapfill = 0;
					}

					for ( k = 0; (collator->state != BAMBAM_COLLATE_FAILED) && k < numtempfiles; ++k )
					{
						#if defined(BAMBAMC_BAMONLY)
						if ( (collator->alignment = BamBam_GzipFileDecoder_DecodeAlignment(collator->mergefiles[k])) )
						#else
						if ( samread(collator->mergefiles[k],collator->alignment) >= 0 )
						#endif
						{
							BamBam_BamCollationHashEntry * nhashentry = 0;
							nhashentry = BamBam_BamCollationHashEntry_NewDup(collator->alignment);
							
							if ( ! nhashentry )
								collator->state = BAMBAM_COLLATE_FAILED;
							else
								BamBam_BamCollator_MergeHeapInsert(collator,nhashentry,k);
						}
						else
						{
							fprintf(stderr,"WARNING: temp %llu file seems empty.\n", (unsigned long long)k);
						}
					}
					
					if ( collator->state != BAMBAM_COLLATE_FAILED )
					{
						/* BamBam_BamCollator_MergeHeapPrint(collator); */
						BamBam_BamCollator_MergeHeapCheck(collator);
					}
				}
			}
		}
		else
		{
			if ( collator->mergeheapfill )
			{
				BamBam_MergeHeapEntry * mini = BamBam_BamCollator_MergeHeapMinimum(collator);
				BamBam_BamCollationHashEntry * hashentry = mini->hashentry;
				uint64_t const fileid = mini->fileid;

				BamBam_BamCollationOutputVector_PushBack(collator->outputvector,hashentry);
				
				mini->hashentry = 0;
				BamBam_BamCollator_MergeHeapDeleteMinimum(collator);

				#if defined(BAMBAMC_BAMONLY)
				if ( (collator->alignment = BamBam_GzipFileDecoder_DecodeAlignment(collator->mergefiles[fileid])) )
				#else
				if ( samread(collator->mergefiles[fileid],collator->alignment) >= 0 )
				#endif
				{
					BamBam_BamCollationHashEntry * nhashentry = 0;
					nhashentry = BamBam_BamCollationHashEntry_NewDup(collator->alignment);
						
					if ( ! nhashentry )
					{
						collator->state = BAMBAM_COLLATE_FAILED;
					}
					else
					{
						BamBam_BamCollator_MergeHeapInsert(collator,nhashentry,fileid);
					}
				}				
			}
			else
				collator->state = BAMBAM_COLLATE_DONE;
		}
	}
	
	if ( collator->outputvector->outputvectorfill )
	{
		#if 0
		int len = -1;
		BamBam_BamCollationHashEntry *entry = BamBam_BamCollationOutputVector_PopFront(collator->outputvector);

		len = BamBam_PutAlignmentBuffer(entry->entry,&(collator->bufferA),&(collator->bufferALength),'\n');

		fwrite(collator->bufferA,1,len,stdout);
                           
		/* fprintf(stdout,"%s\n", entry->qname); */
	 	
		BamBam_BamCollationHashEntry_Delete(entry);
		return 1;
		#endif

		return BamBam_BamCollationOutputVector_PopFront(collator->outputvector);
	}
	else
	{	
		return 0;
	}
}

enum tmpdirstate { BAMBAM_TMPDIR_FAILED = 1, BAMBAM_TMPDIR_CREATED = 2, BAMBAM_TMPDIR_WASPRESENT = 3 };

/* for stat() */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
                    
static int BamBam_CreateTempDir(char const * tmpdirname)
{
	struct stat sb;
	
	int statret = stat(tmpdirname,&sb);
	
	if ( statret == -1 && errno == ENOENT )
	{
		if ( ! mkdir(tmpdirname, 0700) )
		{
			return BAMBAM_TMPDIR_CREATED;
		}
		else
		{
			return BAMBAM_TMPDIR_FAILED;
		}
	}
	/* it exists, check whether it is a directory */
	else
	{
		if ( S_ISDIR(sb.st_mode) )
		{
			return BAMBAM_TMPDIR_WASPRESENT;
		}
		else
		{
			return BAMBAM_TMPDIR_FAILED;
		}
	}
}

static char const * BamBam_BamCollator_InputFormatToMode(char const * inputformat)
{
	if ( strcmp(inputformat,"bam") == 0 )
		return "rb";
	else if ( strcmp(inputformat,"sam") == 0 )
		return "r";
	else
		return 0;
}

BamBam_BamCollator * BamBam_BamCollator_Delete(BamBam_BamCollator * collator)
{
	if ( collator )
	{
		BamBam_BamHeaderInfo_Delete(collator->headerinfo);
		BamBam_BamHeaderInfo_Delete(collator->parsedheaderinfo);
		free(collator->headerline);
		free(collator->bamheadertext);
		free(collator->filteredbamheadertext);
		free(collator->vn);
		free(collator->so);
		free(collator->bufferA);
		free(collator->bufferB);
		uint64_t k;
		if ( collator->mergefiles )
		{
			for ( k = 0; k < collator->nummergefiles; ++k )
				if ( collator->mergefiles[k] )
				{
					#if defined(BAMBAMC_BAMONLY)
					BamBam_GzipFileDecoder_Delete(collator->mergefiles[k]);
					#else
					samclose(collator->mergefiles[k]);
					#endif
					collator->mergefiles[k] = 0;
				}
			free(collator->mergefiles);
			collator->mergefiles = 0;
		}
		if ( collator->mergeheap )
		{
			if ( collator->mergeheapfill )
				BamBam_BamCollator_MergeHeapDeleteMinimum(collator);
			free(collator->mergeheap);
			collator->mergeheap = 0;
		}
		if ( collator->tmpdirstate == BAMBAM_TMPDIR_CREATED )
			rmdir(collator->tempdirname);

		#if !defined(BAMBAMC_BAMONLY)
		if ( collator->alignment )
			bam_destroy1(collator->alignment);
		#endif

		#if defined(BAMBAMC_BAMONLY)
		if ( collator->decoder )
			BamBam_SamBamFileDecoder_Delete(collator->decoder);
		#else
		if ( collator->bamfile )
			samclose(collator->bamfile);
		#endif
		BamBam_BamCollationOutputVector_Delete(collator->outputvector);
		BamBam_BamCollationTempFileGenerator_Delete(collator->gen);
		BamBam_BamCollationVector_Delete(collator->vector);
		BamBam_BamCollationHash_Delete(collator->hash);
		free(collator->tempdirname);
		free(collator->tempprefix);
		BamBam_BamCollationHashEntry_Delete(collator->entryA);
		BamBam_BamCollationHashEntry_Delete(collator->entryB);
		free(collator);
	}
	
	return 0;
}

char const * BamBam_findNextTab(char const * t)
{
	char const * nexttab = t;
			
	while ( *nexttab && *nexttab != '\n' && *nexttab != '\t' )
		++nexttab;

	return nexttab;
}

char const * BamBam_skipTab(char const * t)
{
	if ( *t && *t == '\t' )
		return t+1;
	else
		return t;
}

char const * BamBam_nextField(char const * t)
{
	return BamBam_skipTab(BamBam_findNextTab(t));
}

BamBam_BamCollator * BamBam_BamCollator_New(
	char const * tempdirname,
	unsigned int const hashsizelog,
	unsigned int const vectorsizelog,
	char const * inputformat,
	char const * inputfilename
)
{
	BamBam_BamCollator * collator = 0;
	size_t tempdirnamelen = 0;
	size_t tempfileprefixlen = 0;
	char const * tempadd = "/file";
	char const * mode = BamBam_BamCollator_InputFormatToMode(inputformat);
	char const * hfilters[] = { "HD", "SQ", 0 };
	int i;
		
	tempdirnamelen = strlen(tempdirname);
	tempfileprefixlen = tempdirnamelen + strlen(tempadd) + 1;
	
	collator = (BamBam_BamCollator *)malloc(sizeof(BamBam_BamCollator));

	if ( ! collator )
		return BamBam_BamCollator_Delete(collator);

	memset(collator,0,sizeof(BamBam_BamCollator));
	collator->state = BAMBAM_COLLATE_READING_INPUT;
	collator->tmpdirstate = BamBam_CreateTempDir(tempdirname);

	if ( collator->tmpdirstate == BAMBAM_TMPDIR_FAILED )
		return BamBam_BamCollator_Delete(collator);
	
	collator->tempdirname = strdup(tempdirname);
	
	if ( ! collator->tempdirname )
		return BamBam_BamCollator_Delete(collator);

	collator->tempprefix = (char *)malloc(tempfileprefixlen);	
	
	if ( ! collator->tempprefix )
		return BamBam_BamCollator_Delete(collator);
	
	snprintf(collator->tempprefix,tempfileprefixlen,"%s%s",tempdirname,tempadd);
	
	collator->hash = BamBam_BamCollationHash_New(hashsizelog);
	
	if ( ! collator->hash )
		return BamBam_BamCollator_Delete(collator);
	
	collator->vector = BamBam_BamCollationVector_New(vectorsizelog);
	
	if ( ! collator->vector )
		return BamBam_BamCollator_Delete(collator);

	collator->gen = BamBam_BamCollationTempFileGenerator_New(collator->tempprefix);
	
	if ( ! collator->gen )
		return BamBam_BamCollator_Delete(collator);
	
	collator->outputvector = BamBam_BamCollationOutputVector_New(vectorsizelog);
	
	if ( ! collator->outputvector )
		return BamBam_BamCollator_Delete(collator);

	#if defined(BAMBAMC_BAMONLY)
	collator->decoder = BamBam_SamBamFileDecoder_New(inputfilename,mode);
	
	if ( ! collator->decoder )
		return BamBam_BamCollator_Delete(collator);
	
	collator->bamheadertext = strdup(collator->decoder->header->headertext);
	
	if ( ! collator->bamheadertext )
		return BamBam_BamCollator_Delete(collator);
		
	#else
	collator->bamfile = samopen(inputfilename,mode,0);
	
	if ( ! collator->bamfile )
		return BamBam_BamCollator_Delete(collator);

	if ( ! collator->bamfile->header )
		return BamBam_BamCollator_Delete(collator);
	
	if ( ! collator->bamfile->header->l_text )
		collator->bamfile->header->l_text = strlen(collator->bamfile->header->text);
	
	collator->bamheadertext = malloc(collator->bamfile->header->l_text+1);
		
	if ( ! collator->bamheadertext )
		return BamBam_BamCollator_Delete(collator);
		
	memset(collator->bamheadertext,0,collator->bamfile->header->l_text+1);
	memcpy(collator->bamheadertext,collator->bamfile->header->text,collator->bamfile->header->l_text);
	#endif

	collator->filteredbamheadertext = BamBam_filterHeader(collator->bamheadertext,&hfilters[0]);

	if ( ! collator->filteredbamheadertext )
		return BamBam_BamCollator_Delete(collator);
	
	/* parse header into header info structure, if we have a header text */
	if ( collator->bamheadertext && (strlen(collator->bamheadertext) != 0) )
	{
		char const * s = collator->bamheadertext;
		
		while ( (! collator->headerline) && *s )
		{
			if ( BamBam_getLineLength(s) >= 4 && s[0] == '@' && s[1] == 'H' && s[2] == 'D' && s[3] == '\t' )
			{
				collator->headerline = (char *)malloc(BamBam_getLineLength(s)+1);
				
				if ( ! collator->headerline )
					return BamBam_BamCollator_Delete(collator);
				
				memset(collator->headerline,0,BamBam_getLineLength(s)+1);
				memcpy(collator->headerline,s,BamBam_getLineLength(s));
			}
			
			s = BamBam_nextLine(s);
		}
		
		if ( collator->headerline )
		{
			char const * t = collator->headerline;
			
			while ( *t && *t != '\n' )
			{
				char const * nexttab = BamBam_findNextTab(t);
				
				/* version number */
				if ( 
					nexttab-t >= 3 
					&&
					t[0] == 'V' && t[1] == 'N' && t[2] == ':'
				)
				{
					collator->vn = (char *)malloc((nexttab-t)-3+1);
					
					if ( ! collator->vn )
						return BamBam_BamCollator_Delete(collator);
					
					memset(collator->vn,0,(nexttab-t)-3+1);
					memcpy(collator->vn,t+3,(nexttab-t)-3);
				}
				/* sort order */
				if ( 
					nexttab-t >= 3 
					&&
					t[0] == 'S' && t[1] == 'O' && t[2] == ':'
				)
				{
					collator->so = (char *)malloc((nexttab-t)-3+1);
					
					if ( ! collator->so )
						return BamBam_BamCollator_Delete(collator);
					
					memset(collator->so,0,(nexttab-t)-3+1);
					memcpy(collator->so,t+3,(nexttab-t)-3);					
				}
				
				t = BamBam_nextField(t);
			}
		}

		if ( ! collator->vn )
		{
			collator->vn = strdup("1.4");
			if ( ! collator->vn )
				return BamBam_BamCollator_Delete(collator);
		}
		if ( ! collator->so )
		{
			collator->so = strdup("unknown");
			if ( ! collator->so )
				return BamBam_BamCollator_Delete(collator);
		}

		collator->parsedheaderinfo = BamBam_BamHeaderInfo_New(collator->vn, collator->so, collator->filteredbamheadertext);
		
		if ( ! collator->parsedheaderinfo )
			return BamBam_BamCollator_Delete(collator);

		s = collator->bamheadertext;

		while ( *s )
		{
			if ( BamBam_getLineLength(s) >= 4 && s[0] == '@' && s[1] == 'S' && s[2] == 'Q' && s[3] == '\t' )
			{
				char const * t = s;
				char * sn = 0;
				long ln = -1;
				
				while ( *t && *t != '\n' )
				{
					char const * nexttab = BamBam_findNextTab(t);
					
					/* sequence name */
					if ( 
						nexttab-t >= 3 
						&&
						t[0] == 'S' && t[1] == 'N' && t[2] == ':'
					)
					{
						sn = (char *)malloc((nexttab-t)-3+1);
						if ( ! sn )
							return BamBam_BamCollator_Delete(collator);
						memset(sn,0,(nexttab-t)-3+1);
						memcpy(sn,t+3,(nexttab-t)-3);
					}
					/* sequence length */
					if ( 
						nexttab-t >= 3 
						&&
						t[0] == 'L' && t[1] == 'N' && t[2] == ':'
					)
					{
						ln = 0;
						char const * tt = t+3;
						
						while ( isdigit(*tt) )
						{
							ln *= 10;
							ln += *tt-'0';
							tt++;
						}						
					}
					
					t = BamBam_nextField(t);
				}
				
				if ( sn && ln != -1 )
				{
					if ( BamBam_BamHeaderInfo_AddChromosome(collator->parsedheaderinfo,sn,ln) )
					{
						free(sn);
						return BamBam_BamCollator_Delete(collator);
					}
				}
				
				free(sn);
			}
			
			s = BamBam_nextLine(s);
		}
		
		if ( BamBam_BamHeaderInfo_ProduceHeaderText(collator->parsedheaderinfo) < 0 )
			return BamBam_BamCollator_Delete(collator);
	}


	if ( ! collator->vn )
	{
		collator->vn = strdup("1.4");
		if ( ! collator->vn )
			return BamBam_BamCollator_Delete(collator);
	}
	if ( ! collator->so )
	{
		collator->so = strdup("unknown");
		if ( ! collator->so )
			return BamBam_BamCollator_Delete(collator);
	}

	collator->headerinfo = BamBam_BamHeaderInfo_New(collator->vn, collator->so, collator->filteredbamheadertext);
	
	if ( ! collator->headerinfo )
		return BamBam_BamCollator_Delete(collator);


		
	#if defined(BAMBAMC_BAMONLY)
	for ( i = 0; i < collator->decoder->header->n_ref; ++i )
	{
		if ( BamBam_BamHeaderInfo_AddChromosome(collator->headerinfo,
			collator->decoder->header->chromosomevec[i]->name,collator->decoder->header->chromosomevec[i]->length) 
		)
			return BamBam_BamCollator_Delete(collator);
		
	}
	#else
	for ( i = 0; i < collator->bamfile->header->n_targets; ++i )
	{
		if ( BamBam_BamHeaderInfo_AddChromosome(collator->headerinfo,
			collator->bamfile->header->target_name[i],collator->bamfile->header->target_len[i]) 
		)
			return BamBam_BamCollator_Delete(collator);
	}
	#endif

	if ( BamBam_BamHeaderInfo_ProduceHeaderText(collator->headerinfo) < 0 )
		return BamBam_BamCollator_Delete(collator);

	#if ! defined(BAMBAMC_BAMONLY)
	collator->alignment = bam_init1();
	
	if ( ! collator->alignment )
		return BamBam_BamCollator_Delete(collator);
	#endif
		
	return collator;
}

int BamBam_BamCollator_Get(BamBam_BamCollator * collator, BamBam_BamCollationHashEntry ** entryA, BamBam_BamCollationHashEntry ** entryB)
{
	BamBam_BamCollationHashEntry * peekA = 0;
	BamBam_BamCollationHashEntry * peekB = 0;
	
	*entryA = 0;
	*entryB = 0;

	peekA = BamBam_BamCollator_GetNextRead(collator);
	peekB = BamBam_BamCollator_GetNextRead(collator);
	
	/* no alignments left */
	if ( ! peekA )
	{
		return 0;
	}
	/* only one alignment left (orphan) */
	else if ( ! peekB )
	{
		if ( BamBam_BamCollationHashEntry_GetFlags(peekA) & BAMBAMC_FREAD2 )
			*entryB = peekA;
		else
			*entryA = peekA;
			
		return 1;
	}
	else
	{
		assert ( peekA );
		assert ( peekB );
	
		/* pair */
		if (
			(peekA->hashvalue == peekB->hashvalue)
			&&
			(!strcmp(peekA->qname,peekB->qname))
			&&
			(
			((BamBam_BamCollationHashEntry_GetFlags(peekA) & BAMBAMC_FREAD1) &&(BamBam_BamCollationHashEntry_GetFlags(peekB) & BAMBAMC_FREAD2))
			||
			((BamBam_BamCollationHashEntry_GetFlags(peekB) & BAMBAMC_FREAD1) &&(BamBam_BamCollationHashEntry_GetFlags(peekA) & BAMBAMC_FREAD2))
			)
		)
		{
			if ( BamBam_BamCollationHashEntry_GetFlags(peekA) & BAMBAMC_FREAD1 )
			{
				*entryA = peekA;
				*entryB = peekB;
			}
			else
			{
				*entryA = peekB;
				*entryB = peekA;
			}
			
			return 2;
		}
		/* orphan */
		else
		{
			if ( BamBam_BamCollationHashEntry_GetFlags(peekA) & BAMBAMC_FREAD2 )
				*entryB = peekA;
			else
				*entryA = peekA;
		
			/* return alignment to queue */		
			BamBam_BamCollationOutputVector_PushFront(collator->outputvector,peekB);
			
			return 1;
		}
	}
}
