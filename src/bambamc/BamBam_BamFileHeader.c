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
#include <bambamc/BamBam_BamFileHeader.h>
#include <bambamc/BamBam_LineParsing.h>
#include <bambamc/BamBam_StrDup.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static int chromosomeCompareNames(uint8_t const * unamea, uint8_t const * unameb)
{
	while ( *unamea && *unameb )
	{
		if ( isdigit(*unamea) && isdigit(*unameb) )
		{
			int numa = 0, numb = 0;
			
			while ( *unamea && isdigit(*unamea) )
			{
				numa *= 10;
				numa += (*unamea-'0');
				unamea++;
			}
			while ( *unameb && isdigit(*unameb) )
			{
				numb *= 10;
				numb += (*unameb-'0');
				unameb++;
			}
			
			if ( numa != numb )
				return numa-numb;
		}
		else if ( *unamea != *unameb )
		{
			return (int)(*unamea) - (int)(*unameb);
		}
		else
		{
			unamea++, unameb++;
		}
	}
	
	return (int)(*unamea) - (int)(*unameb);
}

static int chromosomeNameComparison(void const * va, void const * vb)
{
	BamBam_Chromosome * a = *((BamBam_Chromosome**)(va));
	BamBam_Chromosome * b = *((BamBam_Chromosome**)(vb));
	char const * namea = a->name;
	char const * nameb = b->name;
	uint8_t const * unamea = (uint8_t const *)(namea);
	uint8_t const * unameb = (uint8_t const *)(nameb);

	return chromosomeCompareNames(unamea,unameb);
}


BamBam_BamFileHeader * BamBam_BamFileHeader_Delete(BamBam_BamFileHeader * object)
{
	if ( object )
	{
		if ( object->headertext )
		{
			free(object->headertext);
			object->headertext = 0;
		}
		if ( object->sortorder )
		{
			free(object->sortorder);
			object->sortorder = 0;
		}
		if ( object->version )
		{
			free(object->version);
			object->version = 0;
		}
		if ( object->headerlines )
		{
			char ** h = object->headerlines;
			
			for ( ; *h ; ++h )
				free(*h);
			
			free (object->headerlines);
			object->headerlines = 0;
		}
		if ( object->chromosomevec )
		{
			free(object->chromosomevec);
			object->chromosomevec = 0;
		}
		if ( object->sortedchromosomevec )
		{
			free(object->sortedchromosomevec);
			object->sortedchromosomevec = 0;
		}
		if ( object->chromosomes )
		{
			BamBam_List_Delete(object->chromosomes);
		}
		if ( object->text )
		{
			free(object->text);
			object->text = 0;
		}
		free(object);
	}

	return 0;
}

static void chromosomeDelete(void * chromosome)
{
	BamBam_Chromosome_Delete((BamBam_Chromosome *)chromosome);
}

static BamBam_BamFileHeader * parseHeaderText(BamBam_BamFileHeader * header)
{
	int headerlines = 0;
	char const * c = header->text;
	unsigned int i;
	char ** hc = 0;
	
	while ( *c )
	{
		headerlines++;
		c = BamBam_nextLine(c);
	}

	header->headerlines = (char **)malloc((headerlines+1) * sizeof(char const *));

	if ( ! header->headerlines )
	{
		return BamBam_BamFileHeader_Delete(header);
	}

	for ( i = 0; (int)i < headerlines; ++i )
		header->headerlines[i] = 0;
	header->headerlines[headerlines] = 0;
	
	headerlines = 0;
	c = header->text;
	
	while ( *c )
	{
		int const len = BamBam_getLineLength(c);
	
		header->headerlines[headerlines] = malloc(len+1);
		
		if ( ! header->headerlines[headerlines] )
			return BamBam_BamFileHeader_Delete(header);
	
		header->headerlines[headerlines][len] = 0;
		memcpy(header->headerlines[headerlines],c,len);		
		
		headerlines++;
		c = BamBam_nextLine(c);
	}
	
	for ( hc = header->headerlines; *hc; ++hc )
	{
		if ( strlen(*hc) >= 4 && !strncmp("@HD\t",*hc,4) )
			header->hdline = *hc;
	}

	if ( header->hdline )
	{
		for ( c = header->hdline; *c; ++c )
			if (
				c[0] == '\t' &&
				c[1] == 'S' &&
				c[2] == 'O' &&
				c[3] == ':' )
			{
				char const * d = c+4;
				char const * e = d;
				
				while ( *e != 0 && *e != '\t' )
					++e;
				
				if ( e-d )
				{
					header->sortorder = (char *)malloc((e-d)+1);
					if ( ! header->sortorder )
					{
						return BamBam_BamFileHeader_Delete(header);
					}
					header->sortorder[e-d] = 0;
					memcpy(header->sortorder,d,e-d);
				}
			}
			else if (
				c[0] == '\t' &&
				c[1] == 'V' &&
				c[2] == 'N' &&
				c[3] == ':' )
			{
				char const * d = c+4;
				char const * e = d;
				
				while ( *e != 0 && *e != '\t' )
					++e;
				
				if ( e-d )
				{
					header->version = (char *)malloc((e-d)+1);
					if ( ! header->version )
					{
						return BamBam_BamFileHeader_Delete(header);
					}
					header->version[e-d] = 0;
					memcpy(header->version,d,e-d);
				}
			}
	}	
	
	if ( ! header->version )
	{
		header->version = BamBam_StrDup("1.4");
		if ( ! header->version )
			return BamBam_BamFileHeader_Delete(header);
	}
	if ( ! header->sortorder )
	{
		header->sortorder = BamBam_StrDup("unknown");
		if ( ! header->sortorder )
			return BamBam_BamFileHeader_Delete(header);
	}

	return header;
}

static int findChromosomeByName(
	BamBam_Chromosome const ** sc, unsigned int const numchr,
	char const * name
)
{
	int left = 0;
	int right = numchr;

	while ( right > left )
	{
		unsigned int const mid = left + (right-left)/2;		
		int const r = chromosomeCompareNames((uint8_t const *)name,(uint8_t const *)(sc[mid]->name));
		
		if ( r == 0 )
			return mid;
		else if ( r < 0 )
			right = mid;
		else
			left = mid+1;
	}
	
	return -1;
}

int BamBam_BamFileHeader_FindChromosomeIdByName(BamBam_BamFileHeader const * header, char const * name)
{
	return findChromosomeByName((BamBam_Chromosome const **)header->sortedchromosomevec,header->n_ref,name);
}

BamBam_Chromosome const * BamBam_BamFileHeader_FindChromosomeByName(BamBam_BamFileHeader const * header, char const * name)
{
	int const r = BamBam_BamFileHeader_FindChromosomeIdByName(header,name);

	if ( r < 0 )
		return 0;
	else
		return header->sortedchromosomevec[r];
}

BamBam_BamFileHeader * BamBam_BamFileHeader_New_SAM(FILE * reader)
{
	BamBam_BamFileHeader * header = 0;
	BamBam_CharBuffer * buffer = 0;
	int headerTextComplete = 0;
	int r = 0;
	char ** hc = 0;
	BamBam_ListNode * node = 0;
	size_t i;

	header = (BamBam_BamFileHeader *)malloc(sizeof(BamBam_BamFileHeader));
	
	if ( ! header )
		return BamBam_BamFileHeader_Delete(header);

	memset(header,0,sizeof(BamBam_BamFileHeader));
	
	buffer = BamBam_CharBuffer_New();

	if ( ! buffer )
		return BamBam_BamFileHeader_Delete(header);
	
	while ( !headerTextComplete )
	{
		int c = -1;
		
		c = getc(reader);
		
		if ( c < 0 || c != '@' )
		{
			headerTextComplete = 1;
			if ( c >= 0 )
				ungetc(c,reader);
		}
		else
		{
			while ( c >= 0 && c != '\n' )
			{
				BamBam_CharBuffer_PushCharQuick(buffer,c,r);
				
				if ( r < 0 )
				{
					BamBam_CharBuffer_Delete(buffer);	
					return BamBam_BamFileHeader_Delete(header);
				}
				
				c = getc(reader);
			}
			
			if ( c < 0 )
			{
				BamBam_CharBuffer_Delete(buffer);	
				return BamBam_BamFileHeader_Delete(header);
			}

			BamBam_CharBuffer_PushCharQuick(buffer,'\n',r);
				
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(buffer);	
				return BamBam_BamFileHeader_Delete(header);
			}
		}
	}

	BamBam_CharBuffer_PushCharQuick(buffer,0,r);
				
	if ( r < 0 )
	{
		BamBam_CharBuffer_Delete(buffer);	
		return BamBam_BamFileHeader_Delete(header);
	}

	/* fprintf(stderr,"::HEADER::\n%s::ENDHEADER::\n",(char const *)buffer->buffer); */
	
	header->text = (char *)buffer->buffer;
	buffer->buffer = 0;
	header->l_text = buffer->bufferfill;
		
	BamBam_CharBuffer_Delete(buffer);

	header = parseHeaderText(header);
	
	if (! header )
		return BamBam_BamFileHeader_Delete(header);

	header->chromosomes = BamBam_List_New();
	
	if ( !header->chromosomes )
		return BamBam_BamFileHeader_Delete(header);

	for ( hc = header->headerlines; *hc; ++hc )
	{
		if ( strlen(*hc) >= 4 && !strncmp("@SQ\t",*hc,4) )
		{
			BamBam_Chromosome * chr = 0;
			BamBam_ListNode * node = 0;
			char const * c = *hc;
			c += 3;

			char * sn = 0;
			int32_t ln = -1;
			
			while ( *c )
			{
				while ( *c && *c == '\t' )
					++c;
				if ( *c )
				{
					char const * ce = c;
					
					while ( *ce && *ce != '\t' )
						++ce;
						
					if ( ce-c >= 3 && !strncmp(c,"SN:",3) )
					{
						sn = malloc((ce-c)-3+1);
						
						if ( ! sn )
						{
							free(sn); sn = 0;
							return BamBam_BamFileHeader_Delete(header);
						}
						
						sn [ (ce-c)-3 ] = 0;
						memcpy(sn,c+3,(ce-c)-3);						
					}
					else if ( ce-c >= 3 && !strncmp(c,"LN:",3) )
					{
						char const * p = c+3;
						ln = 0;
						while ( p != ce )
						{
							ln*=10;
							ln+=(*(p++))-'0';
						}						
					}
					
					c = ce;
				}				
			}
			
			/* missing information, broken header */
			if ( ! sn || ln < 0 )
			{
				if ( sn )
				{
					free(sn);
					sn = 0;
				}
			
				return BamBam_BamFileHeader_Delete(header);	
			}

			/* fprintf(stderr,"Seq %s %d\n", sn,ln); */
			
			chr = BamBam_Chromosome_New(sn,ln);

			free(sn);
			sn = 0;
			
			if ( ! chr )
			{
				return BamBam_BamFileHeader_Delete(header);
			}
			
			chr->headerline = BamBam_StrDup(*hc);
			
			if ( ! chr->headerline )
			{
				fprintf(stderr,"Failed to allocate memory for sequence meta data in BAM header.\n");
				BamBam_Chromosome_Delete(chr);
				return BamBam_BamFileHeader_Delete(header);						
			}

			node = BamBam_ListNode_New();
		
			if ( ! node )
			{
				fprintf(stderr,"Failed to allocate memory for sequence meta data in BAM header.\n");
				BamBam_Chromosome_Delete(chr);
				return BamBam_BamFileHeader_Delete(header);			
			}
		
			node->entry = chr;
			node->bamBamListFreeFunction = chromosomeDelete;
		
			BamBam_ListNode_PushBack(header->chromosomes,node);
			header->n_ref++;
		}
	}

	header->chromosomevec = (BamBam_Chromosome **)malloc(header->n_ref * sizeof(BamBam_Chromosome *));

	if ( ! header->chromosomevec )
	{
		return BamBam_BamFileHeader_Delete(header);
	}

	header->sortedchromosomevec = (BamBam_Chromosome **)malloc(header->n_ref * sizeof(BamBam_Chromosome *));

	if ( ! header->sortedchromosomevec )
	{
		return BamBam_BamFileHeader_Delete(header);
	}
	
	i = 0;
	for ( node = header->chromosomes->first; node; node = node->next )
	{
		BamBam_Chromosome * chr = (BamBam_Chromosome *)node->entry;
		header->sortedchromosomevec[i] = chr;
		header->chromosomevec[i++] = chr;
	}

	qsort(header->sortedchromosomevec, header->n_ref, sizeof(BamBam_Chromosome *),chromosomeNameComparison);

	#if 0
	for ( i = 0; (int)i < (int)header->n_ref; ++i )
	{
		BamBam_Chromosome * chr = header->sortedchromosomevec[i];
		fprintf(stderr,"seq[%d] = %s %d\n", (int)i, chr->name, (int)chr->length);
	}
	#endif
	
	header->headertext = BamBam_StrDup(header->text);
	
	if ( ! header->headertext )
	{	
		return BamBam_BamFileHeader_Delete(header);
	}

	return header;	
}

BamBam_BamFileHeader * BamBam_BamFileHeader_New_BAM(BamBam_GzipReader * reader)
{
	BamBam_BamFileHeader * header = 0;
	char magic[4];
	static char const expMagic[4] = {'B','A','M',1};
	unsigned int i;
	int l;
	char ** hc = 0;
	BamBam_CharBuffer * htextbuf = 0;

	header = (BamBam_BamFileHeader *)malloc(sizeof(BamBam_BamFileHeader));
	
	if ( ! header )
		return BamBam_BamFileHeader_Delete(header);

	memset(header,0,sizeof(BamBam_BamFileHeader));
	
	magic[0] = BamBam_GzipReader_Getc(reader);
	magic[1] = BamBam_GzipReader_Getc(reader);
	magic[2] = BamBam_GzipReader_Getc(reader);
	magic[3] = BamBam_GzipReader_Getc(reader);
	
	for ( i = 0; i < sizeof(expMagic)/sizeof(expMagic[0]); ++i )
		if ( magic[i] != expMagic[i] )
		{
			fprintf(stderr,"Stream is not a BAM file (magic is wrong).\n");
			return BamBam_BamFileHeader_Delete(header);
		}
		
	if ( BamBam_GzipReader_GetInt32(reader,&(header->l_text)) )
	{
		fprintf(stderr,"Failed to read length of plain text in BAM header.\n");
		return BamBam_BamFileHeader_Delete(header);
	}
	
	header->text = (char *)malloc(header->l_text);
	
	if ( header->l_text && (! header->text) )
	{
		fprintf(stderr,"Failed to allocate memory for plain text in BAM header.\n");
		return BamBam_BamFileHeader_Delete(header);
	}
	
	l = BamBam_GzipReader_Read(reader,header->text,header->l_text);
	
	if ( l != header->l_text )
	{
		fprintf(stderr,"Failed to read plain text in BAM header.\n");
		return BamBam_BamFileHeader_Delete(header);	
	}
	
	/* terminate by zero if not already so */
	if ( (!l) || (header->text[l-1]) )
	{
		char * ztext = malloc(l+1);
		
		if ( ! ztext )
		{
			fprintf(stderr,"Failed to allocate memory for plain text in BAM header.\n");
			return BamBam_BamFileHeader_Delete(header);
		}	
		memcpy(ztext,header->text,l);
		ztext[l] = 0;
		free(header->text);
		header->text = ztext;
		header->l_text = l+1;
		l += 1;
	}

	assert ( l == header->l_text );		
	assert ( (header->l_text != 0) && (header->text[header->l_text-1] == 0) );

	if ( BamBam_GzipReader_GetInt32(reader,&(header->n_ref)) )
	{
		fprintf(stderr,"Failed to read number of references in BAM header.\n");
		return BamBam_BamFileHeader_Delete(header);
	}

	header->chromosomes = BamBam_List_New();
	
	if ( ! header->chromosomes )
	{
		return BamBam_BamFileHeader_Delete(header);
	}
	
	header->chromosomevec = (BamBam_Chromosome **)malloc(header->n_ref * sizeof(BamBam_Chromosome *));

	if ( ! header->chromosomevec )
	{
		return BamBam_BamFileHeader_Delete(header);
	}

	header->sortedchromosomevec = (BamBam_Chromosome **)malloc(header->n_ref * sizeof(BamBam_Chromosome *));

	if ( ! header->sortedchromosomevec )
	{
		return BamBam_BamFileHeader_Delete(header);
	}
	
	for ( i = 0; i < (unsigned int)(header->n_ref); ++i )
	{
		int32_t chrnamelen = -1;
		char * chrname = 0;
		int32_t chrlen = -1;
		BamBam_Chromosome * chr = 0;
		BamBam_ListNode * node = 0;
		
		if ( BamBam_GzipReader_GetInt32(reader,&(chrnamelen)) )
		{
			fprintf(stderr,"Failed to read sequence name length in BAM header.\n");
			return BamBam_BamFileHeader_Delete(header);	
		}
		
		chrname = (char*)malloc(chrnamelen);
		
		if ( ! chrname )
		{
			fprintf(stderr,"Failed to allocate space for chromosome name while reading BAM header.\n");
			return BamBam_BamFileHeader_Delete(header);
		}
		
		if ( BamBam_GzipReader_Read(reader,chrname,chrnamelen) != chrnamelen )
		{
			fprintf(stderr,"Failed to read chromosome name while reading BAM header.\n");
			free(chrname);
			return BamBam_BamFileHeader_Delete(header);
		}
		
		if ( BamBam_GzipReader_GetInt32(reader,&(chrlen)) )
		{
			fprintf(stderr,"Failed to read sequence length in BAM header.\n");
			free(chrname);
			return BamBam_BamFileHeader_Delete(header);	
		}
		
		chr = BamBam_Chromosome_New(chrname,chrlen);
		free(chrname);

		if ( !chr )
		{
			fprintf(stderr,"Failed to allocate memory for sequence meta data in BAM header.\n");
			return BamBam_BamFileHeader_Delete(header);	
		}
		
		node = BamBam_ListNode_New();
		
		if ( ! node )
		{
			fprintf(stderr,"Failed to allocate memory for sequence meta data in BAM header.\n");
			BamBam_Chromosome_Delete(chr);
			return BamBam_BamFileHeader_Delete(header);			
		}
		
		node->entry = chr;
		node->bamBamListFreeFunction = chromosomeDelete;
		
		BamBam_ListNode_PushBack(header->chromosomes,node);
		
		header->chromosomevec[i] = chr;	
		header->sortedchromosomevec[i] = chr;
	}

	qsort(header->sortedchromosomevec, header->n_ref, sizeof(BamBam_Chromosome *),chromosomeNameComparison);

	/* check binary search */
	for ( i = 0; i < (unsigned int)(header->n_ref); ++i )
	{
		int r = -1;
		
		#if 0
		fprintf(stderr,"%s\t%llu\n", header->sortedchromosomevec[i]->name, (unsigned long long)header->sortedchromosomevec[i]->length);
		#endif

		r = findChromosomeByName((BamBam_Chromosome const **)header->sortedchromosomevec,header->n_ref,header->sortedchromosomevec[i]->name);
		assert ( r == (int)i );
		
		/* fprintf(stderr,"%d == %d\n", r, i); */
	}

	header = parseHeaderText(header);
	
	if ( ! header )
		return BamBam_BamFileHeader_Delete(header);

	htextbuf = BamBam_CharBuffer_New();
	
	if ( ! htextbuf )
		return BamBam_BamFileHeader_Delete(header);
		
	if ( header->hdline )
	{
		int r = 0;
		BamBam_CharBuffer * buffer = htextbuf;
		
		BamBam_CharBuffer_PushString(buffer,header->hdline,r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,"\n",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
	}
	else
	{
		int r = 0;
		BamBam_CharBuffer * buffer = htextbuf;
		
		BamBam_CharBuffer_PushString(buffer,"@HD\tVN:",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,header->version,r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,"\tSO:",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,header->sortorder,r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,"\n",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
	}

	#if 0
	for ( i = 0; i < (unsigned int)(header->n_ref); ++i )
	{
		BamBam_Chromosome const * chr = header->chromosomevec[i];
		int r = 0;
		BamBam_CharBuffer * buffer = htextbuf;
		
		BamBam_CharBuffer_PushString(buffer,"@SQ\tSN:",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,chr->name,r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,"\tLN:",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushNumber(buffer,chr->length,r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
		BamBam_CharBuffer_PushString(buffer,"\n",r);
		if ( r < 0 )
		{
			BamBam_CharBuffer_Delete(htextbuf);
			return BamBam_BamFileHeader_Delete(header);	
		}
	}
	#endif

	/* copy rest of the lines without SQ and HD lines */
	for ( hc = header->headerlines; *hc; ++hc )
	{
		/* fprintf(stderr, "Checking %s\n", *hc); */
	
		if ( 
			strlen(*hc) >= 4 
			&& strncmp("@HD\t",*hc,4)
			&& strncmp("@SQ\t",*hc,4)
		)
		{
			int r = 0;
			BamBam_CharBuffer * buffer = htextbuf;
		
			BamBam_CharBuffer_PushString(buffer,*hc,r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}
			BamBam_CharBuffer_PushString(buffer,"\n",r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}	
		}
		else if ( strlen(*hc) >= 4 && strncmp("@SQ\t",*hc,4) == 0 )
		{
			char const * line = *hc;
			
			/* fprintf(stderr,"Here: %s\n", line); */
			
			while ( *line )
			{
				char const * field = line;
				char const * fielde = field;
					
				while ( (*fielde) && (*fielde != '\t') )
					++fielde;
					
				/* fprintf(stderr, "field length %d\n", (fielde-field)); */

				if ( fielde-field >= 3 && field[0] == 'S' && field[1] == 'N' && field[2] == ':' )
				{
					char const * name = field + 3;
					char const * namee = fielde;
					char * cname = (char *)malloc(namee-name+1);
					int r = -1, q = 0;
					BamBam_CharBuffer * buffer = htextbuf;
					
					if ( ! cname )
					{
						BamBam_CharBuffer_Delete(htextbuf);
						return BamBam_BamFileHeader_Delete(header);
					}
					
					memcpy(cname,name,namee-name);
					cname[namee-name] = 0;

					r = findChromosomeByName((BamBam_Chromosome const **)header->sortedchromosomevec,header->n_ref,cname);

					if ( r < 0 )
					{
						fprintf(stderr,"Chromosome %s in text index is not in binary header\n",cname);
						free(cname);
						BamBam_CharBuffer_Delete(htextbuf);
						return BamBam_BamFileHeader_Delete(header);					
					}

					assert ( strcmp(header->sortedchromosomevec[r]->name,cname) == 0 );

					free(cname);
					cname = 0;
					
					header->sortedchromosomevec[r]->headerline = BamBam_StrDup(*hc);
					
					if ( ! header->sortedchromosomevec[r]->headerline )
					{
						fprintf(stderr,"Unable to allocate space for header line\n");
						BamBam_CharBuffer_Delete(htextbuf);
						return BamBam_BamFileHeader_Delete(header);										
					}

					BamBam_CharBuffer_PushString(buffer,*hc,q);
					if ( q < 0 )
					{
						BamBam_CharBuffer_Delete(htextbuf);
						return BamBam_BamFileHeader_Delete(header);	
					}
					BamBam_CharBuffer_PushString(buffer,"\n",q);
					if ( q < 0 )
					{
						BamBam_CharBuffer_Delete(htextbuf);
						return BamBam_BamFileHeader_Delete(header);	
					}	

				}
				
				line = fielde;
				if ( *line )
				{
					assert ( *line == '\t' );
					++line;
				}
			}
		}
	}
	
	for ( i = 0; i < (unsigned int)(header->n_ref); ++i )
		if ( ! header->chromosomevec[i]->headerline )
		{
			/* fprintf(stderr,"Sequence %s is in binary header but not in text\n", header->chromosomevec[i]->name); */
			
			BamBam_Chromosome const * chr = header->chromosomevec[i];
			int r = 0;
			BamBam_CharBuffer * buffer = htextbuf;
			
			BamBam_CharBuffer_PushString(buffer,"@SQ\tSN:",r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}
			BamBam_CharBuffer_PushString(buffer,chr->name,r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}
			BamBam_CharBuffer_PushString(buffer,"\tLN:",r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}
			BamBam_CharBuffer_PushNumber(buffer,chr->length,r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}
			BamBam_CharBuffer_PushString(buffer,"\n",r);
			if ( r < 0 )
			{
				BamBam_CharBuffer_Delete(htextbuf);
				return BamBam_BamFileHeader_Delete(header);	
			}
		}
	
	/* push terminator symbol */
	if ( BamBam_CharBuffer_PushChar(htextbuf,0) < 0 )
	{
		BamBam_CharBuffer_Delete(htextbuf);
		return BamBam_BamFileHeader_Delete(header);	
	}
	
	header->headertext = BamBam_StrDup((char const *)(htextbuf->buffer));

	if ( ! header->headertext )
	{
		BamBam_CharBuffer_Delete(htextbuf);
		return BamBam_BamFileHeader_Delete(header);	
	}
	
	BamBam_CharBuffer_Delete(htextbuf);
	
	/* fprintf(stderr,"%s",header->headertext); */
			
	return header;
}
