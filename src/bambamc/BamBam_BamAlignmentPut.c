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
#include <bambamc/BamBam_BamFlagBase.h>

BamBam_AlignmentPut * BamBam_AlignmentPut_Delete(BamBam_AlignmentPut * aput)
{
	if ( aput )
	{
		if ( aput->calignment )
		{
			BamBam_BamSingleAlignment_Delete(aput->calignment);
			aput->calignment = 0;
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
		
	aput->calignment = BamBam_BamSingleAlignment_New();
	
	if ( ! aput->calignment )
		return BamBam_AlignmentPut_Delete(aput);
	
	return aput;
}

/* reg2bin as defined in sam file format spec */
static int reg2bin(uint32_t beg, uint32_t end)
{
	--end;
	if (beg>>14 == end>>14) return ((1ul<<15)-1ul)/7ul + (beg>>14);
	if (beg>>17 == end>>17) return ((1ul<<12)-1ul)/7ul + (beg>>17);
	if (beg>>20 == end>>20) return ((1ul<<9)-1ul)/7ul  + (beg>>20);
	if (beg>>23 == end>>23) return ((1ul<<6)-1ul)/7ul + (beg>>23);
	if (beg>>26 == end>>26) return ((1ul<<3)-1ul)/7ul + (beg>>26);
	return 0;
}

int BamBam_CharBuffer_PutAlignmentC(
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
	/* buffer */
	BamBam_CharBuffer * buffer = aput->charbuffer;
	BamBam_BamSingleAlignment * alignment = aput->calignment;
	
	int ret = 0;
	uint64_t i = 0;
	uint64_t const qlen = strlen(query);
	uint64_t const quallen = qlen;
	uint64_t const namelen = strlen(name);
	uint64_t ncigar = 0;
	uint64_t bin = 0;
	int64_t endpos = rpos;
	static char const cmap[5] = {1,2,4,8,15};
	
	/*
	 * CharBuffer: buffer,buffersize,bufferfill
	 * BamBam_BamSingleAlignment: data,dataav,dataused
	 */
	assert ( ! buffer->buffer );
	buffer->buffer      = alignment->data;
	buffer->buffersize  = alignment->dataav;
	buffer->bufferfill  = alignment->dataused;
	alignment->data     = 0;
	alignment->dataav   = 0;
	alignment->dataused = 0;

	BamBam_CharBuffer_Reset(buffer);

	/* core data */
	BamBam_CharBuffer_PushLE(buffer,(int32_t)tid,ret); if ( ret < 0 ) return ret;
	BamBam_CharBuffer_PushLE(buffer,(int32_t)rpos,ret); if ( ret < 0 ) return ret;
	/* bin_mq_nl, bin will be written later */
	BamBam_CharBuffer_PushLE(buffer,(uint32_t)((rqual<<8)|(namelen+1)),ret); if ( ret < 0 ) return ret;
	/* flag_nc, number of cigar operations will be written later */
	BamBam_CharBuffer_PushLE(buffer,(uint32_t)(flags<<16),ret); if ( ret < 0 ) return ret;
	/* length of query sequence */
	BamBam_CharBuffer_PushLE(buffer,(int32_t)qlen,ret); if ( ret < 0 ) return ret;
	BamBam_CharBuffer_PushLE(buffer,(int32_t)mtid,ret); if ( ret < 0 ) return ret;
	BamBam_CharBuffer_PushLE(buffer,(int32_t)rmpos,ret); if ( ret < 0 ) return ret;
	BamBam_CharBuffer_PushLE(buffer,(int32_t)isize,ret); if ( ret < 0 ) return ret;
		
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
				op = BAMBAMC_CMATCH;
				endpos += num;
				break;
			case 'I':
				op = BAMBAMC_CINS;
				break;
			case 'D':
				op = BAMBAMC_CDEL;
				endpos += num;
				break;
			case 'N':
				op = BAMBAMC_CREF_SKIP;
				endpos += num;
				break;
			case 'S':
				op = BAMBAMC_CSOFT_CLIP;
				break;
			case 'H':
				op = BAMBAMC_CHARD_CLIP;
				break;
			case 'P':
				op = BAMBAMC_CPAD;
				endpos += num;
				break;
			case '=':
				op = BAMBAMC_CEQUAL;
				endpos += num;
				break;
			case 'X':
				op = BAMBAMC_CDIFF;
				endpos += num;
				break;
			default:
				op = '*';
				break;
		}
		
		cigar++;
		
		/* fprintf(stderr,"op: %u cmatch: %u num: %llu\n", op, BAMBAMC_CMATCH, (unsigned long long)num); */
		
		store = op | (num << 4);
		
		BamBam_CharBuffer_PushLE(buffer,store,ret);
		
		++ncigar;
	}

	/* put number of cigar operations */
	buffer->buffer[12] = ((ncigar>>0) & 0xFFu);
	buffer->buffer[13] = ((ncigar>>8) & 0xFFu);
	
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
		
	/* set bin */
	bin = reg2bin(rpos, endpos);
	buffer->buffer[10] = (bin>>0)&0xFFu;
	buffer->buffer[11] = (bin>>8)&0xFFu;

	/* swap buffers */
	alignment->data     = buffer->buffer;
	alignment->dataav   = buffer->buffersize;
	alignment->dataused = buffer->bufferfill;
	buffer->buffer      = 0;
	buffer->buffersize  = 0;
	buffer->bufferfill  = 0;

	return ret;
}
int BamBam_CharBuffer_PutAuxNumberC(BamBam_AlignmentPut * aput, char const * tag, char const type, void const * pvalue)
{
	/* buffer */
	BamBam_CharBuffer * buffer = aput->charbuffer;
	BamBam_BamSingleAlignment * alignment = aput->calignment;	
	int ret = 0;

	/*
	 * swap buffers
	 */
	assert ( ! buffer->buffer );
	buffer->buffer      = alignment->data;
	buffer->buffersize  = alignment->dataav;
	buffer->bufferfill  = alignment->dataused;
	alignment->data     = 0;
	alignment->dataav   = 0;
	alignment->dataused = 0;
	
	assert ( strlen(tag) == 2 );

	/* put tag */
	BamBam_CharBuffer_PushString(buffer,tag,ret); if ( ret < 0 ) return ret;
	/* put type */
	BamBam_CharBuffer_PushCharQuick(buffer,type,ret); if ( ret < 0 ) return ret;
	
	switch ( type )
	{
		case 'A':
		{
			int8_t const value = *((int8_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 'c':
		{
			int8_t const value = *((int8_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 'C':
		{
			uint8_t const value = *((uint8_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 's':
		{
			int16_t const value = *((int16_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 'S':
		{
			uint16_t const value = *((uint16_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 'i':
		{
			int32_t const value = *((int32_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 'I':
		{
			uint32_t const value = *((uint32_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
		case 'f':
		{
			uint32_t const value = *((uint32_t*)pvalue);
			BamBam_CharBuffer_PushLE(buffer,value,ret);
			break;
		}
	}

	/* swap buffers */
	alignment->data     = buffer->buffer;
	alignment->dataav   = buffer->buffersize;
	alignment->dataused = buffer->bufferfill;
	buffer->buffer      = 0;
	buffer->buffersize  = 0;
	buffer->bufferfill  = 0;
	
	return ret;
}
