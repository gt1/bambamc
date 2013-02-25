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
#include <bambamc/BamBam_BamAlignmentPut.h>

BamBam_AlignmentPut * BamBam_AlignmentPut_Delete(BamBam_AlignmentPut * aput)
{
	if ( aput )
	{
		if ( aput->alignment )
		{
			aput->alignment->data = 0;
			aput->alignment->data_len = 0;
			aput->alignment->m_data = 0;
			bam_destroy1(aput->alignment); aput->alignment = 0;
		}
		BamBam_CharBuffer_Delete(aput->charbuffer); aput->charbuffer = 0;
		free(aput);
	}
	
	return 0;
}

BamBam_AlignmentPut * BamBam_AlignmentPut_New()
{
	BamBam_AlignmentPut * aput = 0;
	
	aput = (BamBam_AlignmentPut *)malloc(sizeof(BamBam_AlignmentPut));
	
	if ( ! aput )
		BamBam_AlignmentPut_Delete(aput);
		
	aput->charbuffer = BamBam_CharBuffer_New();

	if ( ! aput->charbuffer )
		return BamBam_AlignmentPut_Delete(aput);
	
	aput->alignment = bam_init1();
	
	if ( ! aput->alignment )
		return BamBam_AlignmentPut_Delete(aput);
	
	return aput;
}

int BamBam_CharBuffer_PutAlignment(
	BamBam_AlignmentPut * aput,
	/* flags */
	int32_t const flags,
	/* target (chromosome) id */
	int32_t const tid,
	/* position on chromosome (0 based) */
	uint64_t const rpos,
	/* mate target id */
	int32_t const mtid,
	/* position of mate on mate target id */
	uint64_t const rmpos,
	/* sequence name */
	char const * name,
	/* query sequence (read) */
	char const * query,
	/* quality string */
	char const * qual,
	/* cigar operations */
	char const * cigar,
	/* mapping quality */
	int32_t const rqual,
	/* insert size */
	int32_t const isize
	)
{
	/* alignment */
	bam1_t * alignment = aput->alignment;
	/* buffer */
	BamBam_CharBuffer * buffer = aput->charbuffer;

	int ret = 0;
	uint64_t i = 0;
	uint64_t const qlen = strlen(query);
	uint64_t const quallen = qlen;
	uint64_t const namelen = strlen(name);
	static char const cmap[5] = {1,2,4,8,15};
	
	BamBam_CharBuffer_Reset(buffer);

	/* core data */
	alignment->core.tid = tid;
	alignment->core.pos = rpos;
	alignment->core.qual = rqual;
	alignment->core.l_qname = namelen+1;
	alignment->core.flag = flags;
	alignment->core.n_cigar = 0;
	alignment->core.l_qseq = qlen;
	alignment->core.mtid = mtid;
	alignment->core.mpos = rmpos;
	alignment->core.isize = isize;
	
	/* copy read name */
	BamBam_CharBuffer_PushString(buffer,name,ret); if ( ret < 0 ) return ret;
	/* terminating zero */
	BamBam_CharBuffer_PushCharQuick(buffer,0,ret); if ( ret < 0 ) return ret;

	/* if cigar string is "*", then skip it */
	if ( 
		ret >= 0 && 
		cigar[0] && 
		(!cigar[1]) &&
		(cigar[0] == '*')
	)
	{
		++cigar;
	}
	/* encode cigar string */
	while ( ret >= 0 && *cigar )
	{
		uint64_t num = 0;
		uint32_t op = 0;
		uint32_t store = 0;

		assert ( isdigit(cigar[0]) );		
		while ( (*cigar) && isdigit(*cigar) )
		{
			num *= 10;
			num += (*(cigar++)) - '0';
		}
		
		assert ( *cigar );
		switch ( *cigar )
		{
			case 'M':
				op = BAM_CMATCH;
				break;
			case 'I':
				op = BAM_CINS;
				break;
			case 'D':
				op = BAM_CDEL;
				break;
			case 'N':
				op = BAM_CREF_SKIP;
				break;
			case 'S':
				op = BAM_CSOFT_CLIP;
				break;
			case 'H':
				op = BAM_CHARD_CLIP;
				break;
			case 'P':
				op = BAM_CPAD;
				break;
			case '=':
				op = BAM_CEQUAL;
				break;
			case 'X':
				op = BAM_CDIFF;
				break;
			default:
				op = '*';
				break;
		}
		
		cigar++;
		
		/* fprintf(stderr,"op: %u cmatch: %u num: %llu\n", op, BAM_CMATCH, (unsigned long long)num); */
		
		store = op | (num << 4);
		
		BamBam_CharBuffer_PushLE(buffer,store,ret);
		
		alignment->core.n_cigar++;
	}
	
	if ( ret < 0 )
		return ret;
	
	/* encode query string */
	for ( i = 0; i < qlen/2 && ret >= 0; ++i )
	{
		BamBam_CharBuffer_PushCharQuick(
			buffer,
			(cmap[(int)(BamBam_MapBase((int)(query[2*i+0])))] << 4)
			|
			(cmap[(int)(BamBam_MapBase((int)(query[2*i+1])))] << 0),			
			ret);
	}
	if ( qlen % 2 && ret >= 0 )
		BamBam_CharBuffer_PushCharQuick(buffer,((cmap[(int)(BamBam_MapBase((int)(query[qlen-1])))] << 4)),ret);

	if ( ret < 0 )
		return ret;
				
	/* encode quality */
	for ( i = 0; i < quallen && ret >= 0; ++i )
		BamBam_CharBuffer_PushCharQuick(buffer,qual[i]-33,ret);
		
	if ( ret < 0 )
		return ret;	
	
	/* length of mandatory data fields */
	alignment->data_len = buffer->bufferfill;
	/* */
	alignment->m_data = buffer->buffersize;
	/* */ 
	alignment->data = buffer->buffer;
	/* set bin */
	alignment->core.bin = bam_reg2bin(alignment->core.pos, bam_calend(&(alignment->core), bam1_cigar(alignment)));
	
	return ret;

	#if 0	
	#if 0
	putAuxNumber("AS",'i',42);
	#endif
	
	#if 0
	putAuxNumber("XA",'f',42.0f);
	putAuxNumber("XB",'c',5);
	putAuxNumber("XC",'C',6);
	putAuxNumber("XD",'s',7);
	putAuxNumber("XE",'S',8);
	putAuxNumber("XF",'I',16);
	putAuxNumber("XG",'i',-53);
	putAuxString("XH","Hier kommt die Maus");
	
	std::vector<int> V;
	V.push_back(1);
	V.push_back(42);
	V.push_back(11);
	putAuxNumberArray("XI",'i',V);

	putAuxNumber("XJ",'i',-4221);
	putAuxNumberArray("XK",'S',V);
	#endif
	#endif
}

int BamBam_CharBuffer_PutAuxNumber(BamBam_AlignmentPut * aput, char const * tag, char const type, void const * pvalue)
{
	int ret = 0;
	/* alignment */
	bam1_t * alignment = aput->alignment;
	/* buffer */
	BamBam_CharBuffer * buffer = aput->charbuffer;
	
	assert ( strlen(tag) == 2 );

	/* put tag */
	BamBam_CharBuffer_PushString(buffer,tag,ret); if ( ret < 0 ) return ret;
	/* put type */
	BamBam_CharBuffer_PushCharQuick(buffer,type,ret); if ( ret < 0 ) return ret;

	alignment->l_aux += 3;
	alignment->data_len += 3;
	
	switch ( type )
	{
		case 'A':
		{
			int8_t const value = *((int8_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 'c':
		{
			int8_t const value = *((int8_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 'C':
		{
			uint8_t const value = *((uint8_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 's':
		{
			int16_t const value = *((int16_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 'S':
		{
			uint16_t const value = *((uint16_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 'i':
		{
			int32_t const value = *((int32_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 'I':
		{
			uint32_t const value = *((uint32_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
		case 'f':
		{
			uint32_t const value = *((uint32_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			alignment->l_aux += sizeof(value);
			alignment->data_len += sizeof(value);
			break;
		}
	}

	/* length of mandatory data fields */
	alignment->data_len = buffer->bufferfill;
	/* */
	alignment->m_data = buffer->buffersize;
	/* */
	alignment->data = buffer->buffer;
	
	return ret;
}

