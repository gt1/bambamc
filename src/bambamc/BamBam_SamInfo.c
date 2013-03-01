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
#include <bambamc/BamBam_SamInfo.h>
#include <bambamc/BamBam_BamFlagBase.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

BamBam_SamInfo * BamBam_SamInfo_Delete(BamBam_SamInfo * object)
{
	if ( object )
	{
		if ( object->qname )
		{
			free(object->qname);
			object->qname = 0;
		}
		if ( object->rname )
		{
			free(object->rname);
			object->rname = 0;
		}
		if ( object->cigar )
		{
			free(object->cigar);
			object->cigar = 0;
		}
		if ( object->rnext )
		{
			free(object->rnext);
			object->rnext = 0;
		}
		if ( object->seq )
		{
			free(object->seq);
			object->seq = 0;
		}
		if ( object->qual )
		{
			free(object->qual);
			object->qual = 0;
		}
		free(object);
	}
	return 0;
}

BamBam_SamInfo * BamBam_SamInfo_New()
{
	BamBam_SamInfo * object = 0;
	
	object = (BamBam_SamInfo *)malloc(sizeof(BamBam_SamInfo));
	
	if ( ! object )
		return BamBam_SamInfo_Delete(object);
		
	memset(object,0,sizeof(BamBam_SamInfo));
		
	return object;
}

static int BamBam_SamInfo_parseStringField(BamBam_SamInfo_cptrpair field, char ** str, unsigned int * maxlen, bambamc_sam_field_status * defined)
{
	unsigned int const fieldlen = field[1]-field[0];
	
	// undefined by default
	*defined = bambamc_sam_field_undefined;
	
	if ( fieldlen > *maxlen )
	{
		char * newstr = realloc(*str,fieldlen+1);
		if ( ! newstr )
			return -1;
		*str = newstr;
		if ( ! *str )
			return -1;
		*maxlen = fieldlen;
	}
	
	if ( fieldlen == 1 && field[0][0] == '*' )
	{				
		(*str)[0] = '*';
		(*str)[1] = 0;
	}
	else
	{
		memcpy(*str,field[0],fieldlen);
		(*str)[fieldlen] = 0;
		*defined = bambamc_sam_field_undefined;
	}
	
	return 0;
}


static int BamBam_SamInfo_parseNumberField(BamBam_SamInfo_cptrpair field, int32_t * str, bambamc_sam_field_status * defined)
{
	char const * p = field[0];
	uint32_t const fieldlen = field[1]-field[0];
	*defined = bambamc_sam_field_undefined;
	
	if ( fieldlen > 1 && p[0] == '-' )
	{
		int32_t num = 0;
		++p;
		
		while ( p != field[1] )
			if ( isdigit(*p) )
			{
				num *= 10;
				num += (*p-'0');
				++p;
			}
			else
			{
				return -1;
			}
		
		*defined = bambamc_sam_field_defined;	
		*str = -num;
	}
	else
	{
		int32_t num = 0;

		while ( p != field[1] )
			if ( isdigit(*p) )
			{
				num *= 10;
				num += (*p-'0');
				++p;
			}
			else
			{
				return -1;
			}
			
		*defined = bambamc_sam_field_defined;	
		*str = num;		
	}
	
	return 0;
}

static char BamBam_SamInfo_qnameValid[256];
static char BamBam_SamInfo_qnameValidInitialised = 0;
static char BamBam_SamInfo_rnameFirstValid[256];
static char BamBam_SamInfo_rnameFirstValidInitialised = 0;
static char BamBam_SamInfo_rnameOtherValid[256];
static char BamBam_SamInfo_rnameOtherValidInitialised = 0;
static char BamBam_SamInfo_seqValid[256];
static char BamBam_SamInfo_seqValidInitialised = 0;
static char BamBam_SamInfo_qualValid[256];
static char BamBam_SamInfo_qualValidInitialised = 0;
static int BamBam_SamInfo_tablesInitialised = 0;

void BamBam_SamInfo_initSamCharTables()
{
	if ( ! BamBam_SamInfo_tablesInitialised )
	{
		int i = 0;
		memset(&BamBam_SamInfo_qnameValid[0],0,sizeof(BamBam_SamInfo_qnameValid));
		memset(&BamBam_SamInfo_rnameFirstValid[0],0,sizeof(BamBam_SamInfo_rnameFirstValid));
		memset(&BamBam_SamInfo_rnameOtherValid[0],0,sizeof(BamBam_SamInfo_rnameOtherValid));
		memset(&BamBam_SamInfo_seqValid[0],0,sizeof(BamBam_SamInfo_seqValid));
		memset(&BamBam_SamInfo_qualValid[0],0,sizeof(BamBam_SamInfo_qualValid));
		
		for ( i = '!'; i <= '?'; ++i )
			BamBam_SamInfo_qnameValid[i] = 1;
		for ( i = 'A'; i <= '~'; ++i )
			BamBam_SamInfo_qnameValid[i] = 1;
		for ( i = '!'; i <= '('; ++i )
			BamBam_SamInfo_rnameFirstValid[i] = 1;
		BamBam_SamInfo_rnameFirstValid[')'] = 1;
		for ( i = '+'; i <= '<'; ++i )
			BamBam_SamInfo_rnameFirstValid[i] = 1;
		for ( i = '>'; i <= '~'; ++i )
			BamBam_SamInfo_rnameFirstValid[i] = 1;
		for ( i = '!'; i <= '~'; ++i )
			BamBam_SamInfo_rnameOtherValid[i] = 1;
		for ( i = 'A'; i <= 'Z'; ++i )
			BamBam_SamInfo_seqValid[i] = 1;
		for ( i = 'a'; i <= 'z'; ++i )
			BamBam_SamInfo_seqValid[i] = 1;
		BamBam_SamInfo_seqValid['='] = 1;
		BamBam_SamInfo_seqValid['.'] = 1;
		for ( i = '!'; i <= '~'; ++i )
			BamBam_SamInfo_qualValid[i] = 1;

		BamBam_SamInfo_qnameValidInitialised = 1;
		BamBam_SamInfo_rnameFirstValidInitialised = 1;
		BamBam_SamInfo_rnameOtherValidInitialised = 1;
		BamBam_SamInfo_seqValidInitialised = 1;
		BamBam_SamInfo_qualValidInitialised = 1;
		
		BamBam_SamInfo_tablesInitialised = 1;
	}
}

int BamBam_SamInfo_parseSamLine(BamBam_SamInfo_cptrpair * fields, BamBam_SamInfo * saminfo)
{
	if ( BamBam_SamInfo_parseStringField(fields[0], &(saminfo->qname), &(saminfo->qnamemax), &(saminfo->qnamedefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 0: ");
		fwrite(fields[0][0],fields[0][1]-fields[0][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	else
	{
		saminfo->qnamelen = fields[0][1] - fields[0][0];
	}
	if ( BamBam_SamInfo_parseNumberField(fields[1], &(saminfo->flag), &(saminfo->flagdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 1: ");
		fwrite(fields[1][0],fields[1][1]-fields[1][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseStringField(fields[2], &(saminfo->rname), &(saminfo->rnamemax), &(saminfo->rnamedefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 2: ");
		fwrite(fields[2][0],fields[2][1]-fields[2][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseNumberField(fields[3], &(saminfo->pos), &(saminfo->posdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 3: ");
		fwrite(fields[3][0],fields[3][1]-fields[3][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseNumberField(fields[4], &(saminfo->mapq), &(saminfo->mapqdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 4: ");
		fwrite(fields[4][0],fields[4][1]-fields[4][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseStringField(fields[5], &(saminfo->cigar), &(saminfo->cigarmax), &(saminfo->cigardefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 5: ");
		fwrite(fields[5][0],fields[5][1]-fields[5][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseStringField(fields[6], &(saminfo->rnext), &(saminfo->rnextmax), &(saminfo->rnextdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 6: ");
		fwrite(fields[6][0],fields[6][1]-fields[6][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseNumberField(fields[7], &(saminfo->pnext), &(saminfo->pnextdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 7: ");
		fwrite(fields[7][0],fields[7][1]-fields[7][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseNumberField(fields[8], &(saminfo->tlen), &(saminfo->tlendefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 8: ");
		fwrite(fields[8][0],fields[8][1]-fields[8][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	if ( BamBam_SamInfo_parseStringField(fields[9], &(saminfo->seq), &(saminfo->seqmax), &(saminfo->seqdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 9: ");
		fwrite(fields[9][0],fields[9][1]-fields[9][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	else
	{
		saminfo->seqlen = fields[9][1] - fields[9][0];
	}
	if ( BamBam_SamInfo_parseStringField(fields[10], &(saminfo->qual), &(saminfo->qualmax), &(saminfo->qualdefined)) < 0 )
	{
		fprintf(stderr,"Unable to parse column 10: ");
		fwrite(fields[10][0],fields[10][1]-fields[10][0],1,stderr);
		fprintf(stderr,"\n");
		return -1;
	}
	else
	{
		saminfo->seqlen = fields[10][1] - fields[10][0];
	}
	
	if ( saminfo->qnamedefined == bambamc_sam_field_defined )
	{
		int ok = 1;
		char const * p = saminfo->qname;
		
		while ( *p )
		{
			ok = ok && BamBam_SamInfo_qnameValid[(int)*p];
			++p;
		}
		
		if ( !ok )
		{
			fprintf(stderr,"Invalid name %s\n", saminfo->qname);
			return -1;
		}
		if ( p == saminfo->qname )
		{
			fprintf(stderr,"Invalid name %s\n", saminfo->qname);
			return -1;
		}
	}
	if ( saminfo->flagdefined == bambamc_sam_field_defined )
	{
		if ( saminfo->flag < 0 || saminfo->flag >= (int32_t)(1u<<16) )
		{
			fprintf(stderr,"Invalid flag field %d\n", saminfo->flag);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"Invalid undefined flag field.\n");
		return -1;
	}
	if ( saminfo->rnamedefined == bambamc_sam_field_defined )
	{
		int ok = 1;
		char const * p = saminfo->rname;

		if ( !*p )
		{
			fprintf(stderr,"Invalid empty rname field.\n");
			return -1;
		}
			
		ok = ok && BamBam_SamInfo_rnameFirstValid[(int)*p];
		++p;
		
		while ( *p )
		{
			ok = ok && BamBam_SamInfo_rnameOtherValid[(int)*p];
			++p;
		}
		
		if ( ! ok )
		{
			fprintf(stderr,"Invalid rname field %s.\n", saminfo->rname);
			return -1;
		}
	}
	if ( saminfo->posdefined == bambamc_sam_field_defined )
	{
		if ( saminfo->pos < 0 || saminfo->pos >= (int32_t)(1u<<29) )
		{
			fprintf(stderr,"Invalid pos field %d\n", saminfo->pos);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"Invalid undefined pos field\n");
		return -1;
	}
	if ( saminfo->mapqdefined == bambamc_sam_field_defined )
	{
		if ( saminfo->mapq < 0 || saminfo->mapq >= (int32_t)(1u<<8) )
		{
			fprintf(stderr,"Invalid mapping quality %d\n", saminfo->mapq);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"Invalid undefined mapping quality.\n");
		return -1;
	}
	if ( saminfo->cigardefined == bambamc_sam_field_defined )
	{
		char const * p = saminfo->cigar;
		unsigned int exseqlen = 0;
		
		while ( *p )
		{
			unsigned int num = 0;
			
			if ( ! isdigit(*p) )
			{
				fprintf(stderr,"Invalid cigar string: %s\n", saminfo->cigar);
				return -1;
			}
			while ( isdigit(*p) )
			{
				num *= 10;
				num += (*p)-'0';
				++p;
			}
			
			switch ( *(p++) )
			{
				case 'M':
				case 'I':
				case 'S':
				case '=':
				case 'X':
					exseqlen += num;
					break;
				case 'D':
				case 'N':
				case 'H':
				case 'P':
					break;
				default:
					fprintf(stderr,"Invalid cigar operator %c\n", *(p-1));
					return -1;
			}
		}
		
		if ( ! (saminfo->flag & BAMBAMC_FUNMAP) )
		{
			if ( exseqlen != strlen(saminfo->seq) )
			{
				fprintf(stderr,"Invalid cigar string %s for sequence %s\n", saminfo->cigar, saminfo->seq);
				return -1;
			}
		}
	}
	if ( saminfo->rnextdefined == bambamc_sam_field_defined )
	{
		int ok = 1;
		char const * p = saminfo->rnext;

		if ( !*p )
		{
			fprintf(stderr,"Invalid empty rnext field.\n");
			return -1;
		}
			
		if ( *p == '=' && !p[1] )
		{
		
		}
		else
		{
			ok = ok && BamBam_SamInfo_rnameFirstValid[(int)*p];
			++p;
		
			while ( *p )
			{
				ok = ok && BamBam_SamInfo_rnameOtherValid[(int)*p];
				++p;
			}
		
			if ( ! ok )
			{
				fprintf(stderr,"Invalid rnext field %s.\n", saminfo->rnext);
				return -1;
			}
		}
	}
	if ( saminfo->pnextdefined == bambamc_sam_field_defined )
	{
		if ( saminfo->pnext < 0 || saminfo->pnext >= (int32_t)(1u<<29) )
		{
			fprintf(stderr,"Invalid pnext field %d\n", saminfo->pnext);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"Invalid undefined pnext field.\n");
		return -1;
	}
	if ( saminfo->tlendefined == bambamc_sam_field_defined )
	{
		if ( saminfo->tlen < ((-((int32_t)(1u<<29)))+1) || saminfo->tlen >= (int32_t)(1u<<29) )
		{
			fprintf(stderr,"Invalid tlen field %d\n", saminfo->tlen);
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"Invalid undefined tlen field\n");
		return -1;
	}
	if ( saminfo->seqdefined == bambamc_sam_field_defined )
	{
		int ok = 1;
		char const * p = saminfo->seq;
		
		while ( *p )
		{
			ok = ok && BamBam_SamInfo_seqValid[(int)*p];
			++p;
		}
		
		if ( !ok )
		{
			fprintf(stderr,"Invalid sequence string %s\n", saminfo->seq);
			return -1;
		}
		if ( p == saminfo->seq )
		{
			fprintf(stderr,"Invalid empty sequence string %s\n", saminfo->seq);
			return -1;
		}
	}
	if ( saminfo->qualdefined == bambamc_sam_field_defined )
	{
		int ok = 1;
		char const * p = saminfo->qual;
		
		while ( *p )
		{
			ok = ok && BamBam_SamInfo_qualValid[(int)*p];
			++p;
		}
		
		if ( !ok )
		{
			fprintf(stderr,"Invalid quality string %s\n", saminfo->qual);
			return -1;
		}
		if ( p == saminfo->qual )
		{
			fprintf(stderr,"Invalid empty quality string %s\n", saminfo->qual);
			return -1;
		}
	}
	if ( 
		saminfo->qualdefined == bambamc_sam_field_defined 
		&&
		saminfo->seqdefined == bambamc_sam_field_defined 
	)
	{
		if ( saminfo->seqlen != saminfo->quallen )
		{
			fprintf(stderr,"Sequence length %u does not match length of quality string %u\n", saminfo->seqlen, saminfo->quallen);
			return -1;
		}	
	}
	if ( 
		saminfo->seqdefined == bambamc_sam_field_defined
		&&
		saminfo->qualdefined == bambamc_sam_field_undefined
	)
	{
		if ( saminfo->qualmax < saminfo->seqlen )
		{
			char * newqual = realloc(saminfo->qual,saminfo->seqlen+1);
			if ( ! newqual )
			{
				fprintf(stderr,"Failed to allocate memory for quality string.\n");
				return -1;
			}
			saminfo->qual = newqual;
			saminfo->qualmax = saminfo->seqlen;
		}

		saminfo->qual[saminfo->seqlen] = 0;
		memset(saminfo->qual,255,saminfo->seqlen);
	}
	if ( saminfo->rnamedefined && saminfo->rnextdefined && saminfo->rnext && saminfo->rnext[0] == '=' && saminfo->rnext[1] == 0 )
	{
		unsigned int const rnamelen = strlen(saminfo->rname);
		if ( saminfo->rnextmax < rnamelen )
		{
			char * newrnext = realloc(saminfo->rnext,rnamelen+1);
			if ( ! newrnext )
			{
				fprintf(stderr,"Failed to allocate memory for rnext.\n");
				return -1;
			}
			saminfo->rnext = newrnext;
			saminfo->rnextmax = rnamelen;
			strcpy(saminfo->rnext,saminfo->rname);
		}
	}
	
	/*	
	fprintf(stdout,"Got name %s\n", saminfo->qname);
	fprintf(stdout,"Got flag %d\n", saminfo->flag);
	fprintf(stdout,"Got rname %s\n", saminfo->rname);
	fprintf(stdout,"Got pos %d\n", saminfo->pos);
	fprintf(stdout,"Got mapq %d\n", saminfo->mapq);
	fprintf(stdout,"Got cigar %s\n", saminfo->cigar);
	fprintf(stdout,"Got rnext %s\n", saminfo->rnext);
	fprintf(stdout,"Got pnext %d\n", saminfo->pnext);
	fprintf(stdout,"Got tlen %d\n", saminfo->tlen);
	fprintf(stdout,"Got seq %s\n", saminfo->seq);
	fprintf(stdout,"Got qual %s\n", saminfo->qual);
	*/
	
	return 0;
}
