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
#include <bambamc/BamBam_BamSingleAlignment.h>
#include <bambamc/BamBam_CharBuffer.h>
#include <bambamc/BamBam_BamFlagBase.h>
#include <assert.h>

static uint32_t getNumLength(uint32_t num, uint32_t const base)
{
	uint32_t len;
	if ( ! num )
		return 1;
		
	len = 0;
	while ( num )
	{
		len++;
		num /= base;
	}
	
	return len;
}


static int32_t decodeInt(uint8_t const * D, unsigned int length)
{
	int32_t v = 0;
	unsigned int i;
	for ( i = 0; i < length; ++i )
		v |= ((*(D++)) << (i*8));
	return v;
}

static uint32_t decodeUInt(uint8_t const * D, unsigned int length)
{
	uint32_t v = 0;
	unsigned int i;
	for ( i = 0; i < length; ++i )
		v |= ((*(D++)) << (i*8));
	return v;
}


static uint64_t getPrimLengthByType(uint8_t const c)
{
	switch ( c )
	{
		case 'A': case 'c': case 'C': return sizeof(int8_t);
		case 's': case 'S': return sizeof(int16_t);
		case 'i': case 'I': return sizeof(int32_t);
		case 'f':           return sizeof(float);
		default: return 0;
	}
}

static int64_t getAuxLength(uint8_t const * D)
{
	switch ( D[2] )
	{
		case 'A': case 'c': case 'C': case 's': case 'S': case 'i': case 'I': case 'f': return 2+1+getPrimLengthByType(D[2]);
		case 'Z':
		{
			uint64_t len = 2+1;
			D += len;
			while ( *D )
				len++, D++;
			len++;
			return len;
		}
		case 'B':
		{
			uint8_t const eltype = D[3];
			uint32_t const numel = decodeUInt(D+4,4);
			return 2/*tag*/+1/*B*/+1/*type*/+4/* array length */+numel*getPrimLengthByType(eltype);
		}
		default:
		{
			return -1;
		}
	}
}
union numberpun
{
	float fvalue;
	uint32_t uvalue;
};


static int auxValueToString(uint8_t const * D, BamBam_CharBuffer * buffer)
{
	int r = 0;
	
	switch ( D[2] )
	{
		case 'A': 
		{
			r = BamBam_CharBuffer_PushChar(buffer,D[3]);
			break;
		}
		case 'c':
		{
			int8_t const v = ((int8_t const *)(D+3))[0];
			if ( r >= 0 && v < 0 )
				BamBam_CharBuffer_PushCharQuick(buffer,'-',r);
			if ( r >= 0 )
				BamBam_CharBuffer_PushNumber(buffer,(v>=0)?v:-v,r);
			break;
		}
		case 'C':
		{
			uint8_t const v = (D+3)[0];
			if ( r >= 0 )
				BamBam_CharBuffer_PushNumber(buffer,v,r);
			break;
		}
		case 's':
		{
			int16_t const v = decodeInt(D+3,2);
			if ( r >= 0 && v < 0 )
				BamBam_CharBuffer_PushCharQuick(buffer,'-',r);
			if ( r >= 0 )
				BamBam_CharBuffer_PushNumber(buffer,(v>=0)?v:-v,r);
			break;
		}
		case 'S':
		{
			uint16_t const v = decodeUInt(D+3,2);
			if ( r >= 0 )
				BamBam_CharBuffer_PushNumber(buffer,v,r);
			break;
		}
		case 'i':
		{
			int32_t const v = decodeInt(D+3,4);
			if ( r >= 0 && v < 0 )
				BamBam_CharBuffer_PushCharQuick(buffer,'-',r);
			if ( r >= 0 )
				BamBam_CharBuffer_PushNumber(buffer,(v>=0)?v:-v,r);
			break;
		}
		case 'I':
		{
			uint32_t const v = decodeUInt(D+3,4);
			if ( r >= 0 )
				BamBam_CharBuffer_PushNumber(buffer,v,r);
			break;
		}
		case 'f':
		{
			static size_t const tmpsize = 32;
			uint32_t const u = decodeUInt(D+3,4);
			float v;
			char * tmpmem = 0;
			union numberpun np;
			np.uvalue = u;
			v = np.fvalue;

			tmpmem = (char *)alloca(tmpsize);
			memset(tmpmem,0,tmpsize);
			snprintf(tmpmem,tmpsize-1,"%f",v);
			
			BamBam_CharBuffer_PushString(buffer,tmpmem,r);
			
			break;
		}
		case 'Z':
		{
			uint8_t const * p = D+3;
			while ( *p && (r >= 0) )
				BamBam_CharBuffer_PushCharQuick(buffer,*(p++),r);
			break;
		}
		case 'B':
		{
			uint8_t const type = D[3];
			uint32_t const len = decodeUInt(D+4,4);
			uint8_t const * p = D+8;
			uint64_t i;

			if ( r >= 0 )
				BamBam_CharBuffer_PushCharQuick(buffer,type,r);
			
			for ( i = 0; (r >= 0) && i < len; ++i )
			{
				if ( r >= 0 )
					BamBam_CharBuffer_PushCharQuick(buffer,',',r);
					
				switch ( type )
				{
					case 'A': 
					{
						r = BamBam_CharBuffer_PushChar(buffer,*p);
						p += 1;
						break;
					}
					case 'c':
					{
						int8_t const v = ((int8_t const *)p)[0];
						if ( r >= 0 && v < 0 )
							BamBam_CharBuffer_PushCharQuick(buffer,'-',r);
						if ( r >= 0 )
							BamBam_CharBuffer_PushNumber(buffer,(v>=0)?v:-v,r);
						p += 1;
						break;
					}
					case 'C':
					{
						uint8_t const v = p[0];
						if ( r >= 0 )
							BamBam_CharBuffer_PushNumber(buffer,v,r);
						p += 1;
						break;
					}
					case 's':
					{
						int16_t const v = decodeInt(p,2);
						if ( r >= 0 && v < 0 )
							BamBam_CharBuffer_PushCharQuick(buffer,'-',r);
						if ( r >= 0 )
							BamBam_CharBuffer_PushNumber(buffer,(v>=0)?v:-v,r);
						p += 2;
						break;
					}
					case 'S':
					{
						uint16_t const v = decodeUInt(p,2);
						if ( r >= 0 )
							BamBam_CharBuffer_PushNumber(buffer,v,r);
						p += 2;
						break;
					}
					case 'i':
					{
						int32_t const v = decodeInt(p,4);
						if ( r >= 0 && v < 0 )
							BamBam_CharBuffer_PushCharQuick(buffer,'-',r);
						if ( r >= 0 )
							BamBam_CharBuffer_PushNumber(buffer,(v>=0)?v:-v,r);
						p += 4;
						break;
					}
					case 'I':
					{
						uint32_t const v = decodeUInt(p,4);
						if ( r >= 0 )
							BamBam_CharBuffer_PushNumber(buffer,v,r);
						p += 4;
						break;
					}
					case 'f':
					{
						static size_t const tmpsize = 32;
						uint32_t const u = decodeUInt(p,4);
						float v;
						char * tmpmem = 0;
						union numberpun np;
						np.uvalue = u;
						v = np.fvalue;

						tmpmem = (char *)alloca(tmpsize);
						memset(tmpmem,0,tmpsize);
						snprintf(tmpmem,tmpsize-1,"%f",v);
						
						BamBam_CharBuffer_PushString(buffer,tmpmem,r);
						
						p += 4;
						
						break;
					}
					case 'Z':
					{
						while ( *p && (r >= 0) )
							BamBam_CharBuffer_PushCharQuick(buffer,*(p++),r);
						if ( r >= 0 )
						{
							assert ( ! *p );
						}
						p += 1;
						break;
					}
				}
			}				
			break;
		}
		/* unknown data type */
		default:
		{
			r = -1;
		}
	}
	
	return r;
}

static int auxToString(uint8_t const * D, BamBam_CharBuffer * buffer)
{
	int r = 0;
	
	if ( r >= 0 )
		BamBam_CharBuffer_PushCharQuick(buffer,D[0],r);
	if ( r >= 0 )
		BamBam_CharBuffer_PushCharQuick(buffer,D[1],r);
	if ( r >= 0 )
		BamBam_CharBuffer_PushCharQuick(buffer,':',r);
	if ( r >= 0 )
		BamBam_CharBuffer_PushCharQuick(buffer,D[2],r);
	if ( r >= 0 )
		BamBam_CharBuffer_PushCharQuick(buffer,':',r);
	if ( r >= 0 )
		r = auxValueToString(D,buffer);
		
	return r;
}

int BamBam_BamSingleAlignment_DecodeAuxSingle(
	BamBam_BamSingleAlignment * algn, 
	uint8_t const * p,
	int reset)
{
	int r = 0;
	
	if ( reset )
		BamBam_CharBuffer_Reset(algn->auxbuffer);

	if ( r >= 0 )
		r = auxToString(p,algn->auxbuffer);
	
	return r;
}

char const * BamBam_BamSingleAlignment_DecodeAux(BamBam_BamSingleAlignment * algn)
{
	int r = 0;
	int first = 1;
	uint8_t const * data = BamBam_BamSingleAlignment_GetEncodedAux(algn);
	uint8_t const * datae = algn->data + algn->dataused;
	
	BamBam_CharBuffer_Reset(algn->auxbuffer);
	
	while ( r >= 0 && data < datae && *data )
	{
		if ( first )
		{
			first = 0;
		}
		else
		{
			if ( r >= 0 )
				r = BamBam_CharBuffer_PushChar(algn->auxbuffer,'\t');
		}
		if ( r >= 0 )
			r = BamBam_BamSingleAlignment_DecodeAuxSingle(algn,data,0);
		data += getAuxLength(data);
	}
	
	if ( r >= 0 )
	{
		r = BamBam_CharBuffer_PushChar(algn->auxbuffer,0);
		algn->auxbuffer->bufferfill -= 1;
	}
	
	if ( r >= 0 )
		return (char const *)algn->auxbuffer->buffer;
	else
		return 0;
}

uint8_t const * BamBam_BamSingleAlignment_FindAux(BamBam_BamSingleAlignment const * algn, char const tag[2])
{
	uint8_t const * data = BamBam_BamSingleAlignment_GetEncodedAux(algn);
	uint8_t const * datae = algn->data + algn->dataused;
		
	while ( data < datae && *data )
	{
		if ( data[0] == tag[0] && data[1] == tag[1] )
			return data;	
		data += getAuxLength(data);
	}
	
	return 0;
}

uint64_t BamBam_BamSingleAlignment_GetAuxLength(BamBam_BamSingleAlignment const * algn)
{
	uint8_t const * dataa = BamBam_BamSingleAlignment_GetEncodedAux(algn);
	uint8_t const * data = dataa;
	uint8_t const * datae = algn->data + algn->dataused;
		
	while ( data < datae && *data )
		data += getAuxLength(data);
	
	return data-dataa;
}

static uint32_t getCigarStringLength(uint8_t const * D, uint32_t ncigar)
{
	uint32_t length = 0;
	uint32_t i;
	#if 0
	static char const * opc = "MIDNSHP=X???????";
	#endif
	
	for ( i = 0; i < ncigar; ++i, D+=4 )
	{
		uint32_t const oppair = decodeUInt(D,4);
		uint32_t const len = (oppair>>4)&((1ul <<28)-1);
		#if 0
		uint32_t const op = oppair & 0xF;
		uint8_t const c = op[opc];
		#endif
		length += (1+getNumLength(len,10));
	}
	
	return length;
}


int32_t BamBam_BamSingleAlignment_GetRefId(BamBam_BamSingleAlignment const * data)
{
	return decodeInt(data->data + 0, 4);
}

int32_t BamBam_BamSingleAlignment_GetPos(BamBam_BamSingleAlignment const * data)
{
	return decodeInt(data->data + 4, 4);
}

uint32_t BamBam_BamSingleAlignment_GetBinMQNL(BamBam_BamSingleAlignment const * data)
{
	return decodeUInt(data->data + 8, 4);
}

uint32_t BamBam_BamSingleAlignment_GetBin(BamBam_BamSingleAlignment const * data)
{
	return (BamBam_BamSingleAlignment_GetBinMQNL(data) >> 16) & 0xFFFFU;
}

uint32_t BamBam_BamSingleAlignment_GetMQ(BamBam_BamSingleAlignment const * data)
{
	return (BamBam_BamSingleAlignment_GetBinMQNL(data) >> 8) & 0xFFU;
}

uint32_t BamBam_BamSingleAlignment_GetNL(BamBam_BamSingleAlignment const * data)
{
	return (BamBam_BamSingleAlignment_GetBinMQNL(data) >> 0) & 0xFFU;
}

uint32_t BamBam_BamSingleAlignment_GetFlagNC(BamBam_BamSingleAlignment const * data)
{
	return decodeUInt(data->data + 12, 4);
}

uint32_t BamBam_BamSingleAlignment_GetFlags(BamBam_BamSingleAlignment const * data)
{
	return (BamBam_BamSingleAlignment_GetFlagNC(data)>>16)&0xFFFFUL;
}

uint32_t BamBam_BamSingleAlignment_GetNC(BamBam_BamSingleAlignment const * data)
{
	return (BamBam_BamSingleAlignment_GetFlagNC(data)>>0)&0xFFFFUL;
}

int32_t BamBam_BamSingleAlignment_GetLSeq(BamBam_BamSingleAlignment const * data)
{
	return decodeInt(data->data + 16, 4);
}

int32_t BamBam_BamSingleAlignment_GetNextRefID(BamBam_BamSingleAlignment const * data)
{
	return decodeInt(data->data + 20, 4);
}

int32_t BamBam_BamSingleAlignment_GetNextPos(BamBam_BamSingleAlignment const * data)
{
	return decodeInt(data->data + 24, 4);
}

int32_t BamBam_BamSingleAlignment_GetTLen(BamBam_BamSingleAlignment const * data)
{
	return decodeInt(data->data + 28, 4);
}

char const * BamBam_BamSingleAlignment_GetReadName(BamBam_BamSingleAlignment const * data)
{
	return (char const *)(data->data + 32);
}

uint8_t const * BamBam_BamSingleAlignment_GetEncodedCigar(BamBam_BamSingleAlignment const * data)
{
	return ((uint8_t const *)BamBam_BamSingleAlignment_GetReadName(data)) + BamBam_BamSingleAlignment_GetNL(data);
}

uint8_t const * BamBam_BamSingleAlignment_GetEncodedQuery(BamBam_BamSingleAlignment const * data)
{
	return BamBam_BamSingleAlignment_GetEncodedCigar(data) + 4 * BamBam_BamSingleAlignment_GetNC(data);
}

uint8_t const * BamBam_BamSingleAlignment_GetEncodedQual(BamBam_BamSingleAlignment const * data)
{
	return BamBam_BamSingleAlignment_GetEncodedQuery(data) + (BamBam_BamSingleAlignment_GetLSeq(data)+1)/2;
}

uint8_t const * BamBam_BamSingleAlignment_GetEncodedAux(BamBam_BamSingleAlignment const * data)
{
	return BamBam_BamSingleAlignment_GetEncodedQual(data) + (BamBam_BamSingleAlignment_GetLSeq(data));
}

int BamBam_BamSingleAlignment_DecodeQueryRc(BamBam_BamSingleAlignment * algn, int const rc)
{
	/* length of query */
	int32_t lseq = BamBam_BamSingleAlignment_GetLSeq(algn);
	int32_t i;
	static char const * mapping  = "=ACMGRSVTWYHKDBN";
	static char const * rmapping = "=TGKCYSBAWRDMHVN";
	
	/*
	 * reverse complements for ambiguity codes:
	 * M={A,C}   -> K={G,T}
	 * R={A,G}   -> Y={C,T}
	 * S={C,G}   -> S={C,G}
	 * V={A,C,G} -> B={C,G,T}
	 * W={A,T}   -> W={A,T}
	 * Y={C,T}   -> R={A,G}
	 * H={A,C,T} -> D={A,G,T}
	 * K={G,T}   -> M={A,C}
	 * D={A,G,T} -> H={A,C,T}
	 * B={C,G,T} -> V={A,C,G}
	 */
	
	if ( lseq > algn->queryspace )
	{
		free(algn->query);
		algn->query = 0;
		algn->query = (char *)malloc(lseq+1);
		if ( ! algn->query )
			return -1;
		algn->queryspace = lseq;
	}
	algn->query[lseq] = 0;
	
	if ( ! rc )
	{
		char * out = algn->query;
		uint8_t const * in = BamBam_BamSingleAlignment_GetEncodedQuery(algn);
		
		for ( i = 0; i < lseq/2; ++i, ++in )
		{
			*(out++) = mapping[(int)((*in) >> 4) & 0xF];
			*(out++) = mapping[(int)((*in) >> 0) & 0xF];
		}
		
		if ( lseq & 1 )
			*(out++) = mapping[(int)((*in) >> 4) & 0xF];
	}
	else
	{
		char * out = algn->query + lseq;
		uint8_t const * in = BamBam_BamSingleAlignment_GetEncodedQuery(algn);

		for ( i = 0; i < lseq/2; ++i, ++in )
		{
			*(--out) = rmapping[(int)((*in) >> 4) & 0xF];
			*(--out) = rmapping[(int)((*in) >> 0) & 0xF];
		}
		
		if ( lseq & 1 )
			*(--out) = rmapping[(int)((*in) >> 4) & 0xF];
	}

	return lseq;
}

int BamBam_BamSingleAlignment_DecodeQuery(BamBam_BamSingleAlignment * algn)
{
	int const rc = (BamBam_BamSingleAlignment_GetFlags(algn) & BAMBAMC_FREVERSE) != 0;
	return BamBam_BamSingleAlignment_DecodeQueryRc(algn,rc);
}

int BamBam_BamSingleAlignment_DecodeQualRc(BamBam_BamSingleAlignment * algn, int const rc)
{
	int32_t lseq = BamBam_BamSingleAlignment_GetLSeq(algn);
	int32_t i;
	
	if ( lseq > algn->qualspace )
	{
		free(algn->qual);
		algn->qual = 0;
		algn->qual = (char *)malloc(lseq+1);
		if ( ! algn->qual )
			return -1;
		algn->qualspace = lseq;
	}
	algn->qual[lseq] = 0;
	
	if ( ! rc )
	{
		char * out = algn->qual;
		uint8_t const * in = BamBam_BamSingleAlignment_GetEncodedQual(algn);
		
		for ( i = 0; i < lseq; ++i )
			*(out++) = *(in++)+33;
	}
	else
	{
		char * out = algn->qual + lseq;
		uint8_t const * in = BamBam_BamSingleAlignment_GetEncodedQual(algn);

		for ( i = 0; i < lseq; ++i )
			*(--out) = (*(in++))+33;
	}

	return lseq;
}

int BamBam_BamSingleAlignment_DecodeQual(BamBam_BamSingleAlignment * algn)
{
	int const rc = (BamBam_BamSingleAlignment_GetFlags(algn) & BAMBAMC_FREVERSE) != 0;
	return BamBam_BamSingleAlignment_DecodeQualRc(algn,rc);
}

int BamBam_BamSingleAlignment_DecodeCigarRc(BamBam_BamSingleAlignment * algn, int const rc)
{
	uint8_t const * ecigar = BamBam_BamSingleAlignment_GetEncodedCigar(algn);
	uint32_t const ncigar = BamBam_BamSingleAlignment_GetNC(algn);
	int32_t const cigstrlen = getCigarStringLength(ecigar,ncigar);
	uint32_t i;
	static char const * opc = "MIDNSHP=X???????";

	if ( cigstrlen > algn->cigarspace )
	{
		free(algn->cigar);
		algn->cigar = 0;
		algn->cigar = (char *)malloc(cigstrlen+1);
		if ( ! algn->cigar )
			return -1;
		algn->cigarspace = cigstrlen;
	}
	algn->cigar[cigstrlen] = 0;
	
	char * out = algn->cigar;
	
	if ( ! rc )
		for ( i = 0; i < ncigar; ++i, ecigar += 4 )
		{
			uint32_t const oppair = decodeUInt(ecigar,4);
			uint32_t len = (oppair>>4)&((1ul <<28)-1);
			uint32_t const op = oppair & 0xF;
			uint8_t  const c = op[opc];
			
			if ( !len )
				(*(out++)) = '0';
			else
			{
				uint32_t const numlen = getNumLength(len,10);
				char * pout = out + numlen;
				uint32_t j;
				for ( j = 0; j < numlen; ++j )
				{
					*(--pout) = '0'+(len%10);
					len /= 10;
				}
				out += numlen;
			}
			
			*(out++) = c;
		}
	else
	{
		ecigar += 4*ncigar;
		
		for ( i = 0; i < ncigar; ++i )
		{
			ecigar -= 4;
			uint32_t const oppair = decodeUInt(ecigar,4);
			uint32_t len = (oppair>>4)&((1ul <<28)-1);
			uint32_t const op = oppair & 0xF;
			uint8_t  const c = op[opc];
			
			if ( !len )
				(*(out++)) = '0';
			else
			{
				uint32_t const numlen = getNumLength(len,10);
				char * pout = out + numlen;
				uint32_t j;
				for ( j = 0; j < numlen; ++j )
				{
					*(--pout) = '0'+(len%10);
					len /= 10;
				}
				out += numlen;
			}
			
			*(out++) = c;
		}
	}


	return cigstrlen;
}

int BamBam_BamSingleAlignment_DecodeCigar(BamBam_BamSingleAlignment * algn)
{
	int const rc = (BamBam_BamSingleAlignment_GetFlags(algn) & BAMBAMC_FREVERSE) != 0;
	return BamBam_BamSingleAlignment_DecodeCigarRc(algn,rc);
}

int32_t BamBam_BamSingleAlignment_DecodeQueryQualCigarRc(
	BamBam_BamSingleAlignment * algn, int rc, int32_t * querylen, int32_t * cigarlen)
{
	int32_t r = 0;
	
	if ( r >= 0 )
	{
		r = BamBam_BamSingleAlignment_DecodeQueryRc(algn,rc);
		*querylen = r;
	}
	if ( r >= 0 )
	{
		r = BamBam_BamSingleAlignment_DecodeQualRc(algn,rc);
		if ( r != *querylen )
			r = -1;
	}
	if ( r >= 0 )
	{
		r = BamBam_BamSingleAlignment_DecodeCigarRc(algn,rc);
		*cigarlen = r;
	}
	
	return r;
}


int32_t BamBam_BamSingleAlignment_DecodeQueryQualCigar(
	BamBam_BamSingleAlignment * algn, int32_t * querylen, int32_t * cigarlen
)
{
	int const rc = (BamBam_BamSingleAlignment_GetFlags(algn) & BAMBAMC_FREVERSE) != 0;
	return BamBam_BamSingleAlignment_DecodeQueryQualCigarRc(algn,rc,querylen,cigarlen);
}

int BamBam_BamSingleAlignment_StoreAlignment(BamBam_BamSingleAlignment const * data, BamBam_GzipWriter * writer)
{
	/* put length of entry */
	if ( BamBam_GzipWriter_PutInt32(writer,data->dataused) < 0 )
		return -1;
	/* put entry */
	if ( BamBam_GzipWriter_Write(writer,(char const *)(data->data),data->dataused) != (int64_t)(data->dataused) )
		return -1;
		
	return 0;
}

int BamBam_BamSingleAlignment_StoreAlignmentBgzf(BamBam_BamSingleAlignment const * data, BamBam_BgzfCompressor * writer)
{
	uint8_t used[4] =
	{
		(data->dataused >> 0) & 0xFFu,
		(data->dataused >> 8) & 0xFFu,
		(data->dataused >> 16) & 0xFFu,
		(data->dataused >> 24) & 0xFFu
	};
	if ( BamBam_BgzfCompressor_Write(writer,&used[0],4) < 0 )
		return -1;
	if ( BamBam_BgzfCompressor_Write(writer,data->data,data->dataused) < 0 )
		return -1;
	
	return 0;
}

int BamBam_BamSingleAlignment_LoadAlignment(BamBam_BamSingleAlignment * data, BamBam_GzipReader * reader)
{
	int32_t reclen;

	if ( BamBam_GzipReader_Peek(reader) < 0 )
	{
		/* fprintf(stderr,"EOF.\n"); */
		return 0;
	}
	
	if ( BamBam_GzipReader_GetInt32(reader,&reclen) < 0 )
		return -1;

	if ( (int32_t)data->dataav < reclen )
	{
		free(data->data);
		data->data = (uint8_t *)malloc(reclen);
		if ( ! data->data )
			return -1;
		data->dataav = reclen;
	}
	
	if ( BamBam_GzipReader_Read(reader,(char*)data->data,reclen) != reclen )
		return -1;
		
	data->dataused = reclen;

	return 1;
}

BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_Delete(BamBam_BamSingleAlignment * data)
{
	if ( data )
	{
		if ( data->auxbuffer )
		{
			BamBam_CharBuffer_Delete(data->auxbuffer);
			data->auxbuffer = 0;
		}
		if ( data->cigar )
		{
			free(data->cigar);
			data->cigar = 0;
			data->cigarspace = 0;
		}
		if ( data->query )
		{
			free(data->query);
			data->query = 0;
			data->queryspace = 0;
		}
		if ( data->qual )
		{
			free(data->qual);
			data->qual = 0;
			data->qualspace = 0;
		}
		if ( data->data )
		{
			free(data->data);
			data->data = 0;
		}
		free(data);
	}
	return 0;
}

BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_New()
{
	BamBam_BamSingleAlignment * data = 0;
	
	data = (BamBam_BamSingleAlignment *)malloc(sizeof(BamBam_BamSingleAlignment));
	
	if ( ! data )
		return BamBam_BamSingleAlignment_Delete(data);
		
	memset(data,0,sizeof(BamBam_BamSingleAlignment));
	
	data->auxbuffer = BamBam_CharBuffer_New();
	
	if ( ! data->auxbuffer )
		return BamBam_BamSingleAlignment_Delete(data);
			
	return data;
}

BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_NewClone(uint8_t const * block, uint32_t const blocksize)
{
	BamBam_BamSingleAlignment * data = 0;
	
	data = (BamBam_BamSingleAlignment *)malloc(sizeof(BamBam_BamSingleAlignment));
	
	if ( ! data )
		return BamBam_BamSingleAlignment_Delete(data);
		
	memset(data,0,sizeof(BamBam_BamSingleAlignment));
	
	data->auxbuffer = BamBam_CharBuffer_New();
	
	if ( ! data->auxbuffer )
		return BamBam_BamSingleAlignment_Delete(data);
		
	if ( blocksize )
	{
		data->data = (uint8_t *)malloc(blocksize);
		
		if ( ! data->data )
			return BamBam_BamSingleAlignment_Delete(data);
			
		memcpy(data->data,block,blocksize);
		data->dataav = blocksize;
		data->dataused = blocksize;
	}
			
	return data;
}

BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_Clone(BamBam_BamSingleAlignment const * o)
{
	BamBam_BamSingleAlignment * data = 0;
	
	/* return null if o is null */
	if ( ! o )
		return 0;
		
	/* return null if o claims to have space for alignment data but pointer is null */
	if ( (!(o->data)) && o->dataav )
		return 0;
	
	data = (BamBam_BamSingleAlignment *)malloc(sizeof(BamBam_BamSingleAlignment));
	
	if ( ! data )
		return BamBam_BamSingleAlignment_Delete(data);
		
	/* erase newly allocated space */
	memset(data,0,sizeof(BamBam_BamSingleAlignment));
	
	/* copy alignment data if there is any */
	if ( o->data )
	{
		data->dataav   = o->dataav;
		data->dataused = o->dataused;
		data->data     = (uint8_t *)malloc(data->dataav);
	
		if ( ! data->data )
			return BamBam_BamSingleAlignment_Delete(data);
		
		memcpy(data->data,o->data,data->dataav);
	}
	
	if ( o->query )
	{
		data->query = (char *)malloc(o->queryspace);
		if ( ! data->query )
			return BamBam_BamSingleAlignment_Delete(data);
		memcpy(data->query,o->query,o->queryspace);
		data->queryspace = o->queryspace;
	}
	if ( o->qual )
	{
		data->qual = (char *)malloc(o->qualspace);
		if ( ! data->qual )
			return BamBam_BamSingleAlignment_Delete(data);
		memcpy(data->qual,o->qual,o->qualspace);
		data->qualspace = o->qualspace;
	}
	if ( o->cigar )
	{
		data->cigar = (char *)malloc(o->cigarspace);
		if ( ! data->cigar )
			return BamBam_BamSingleAlignment_Delete(data);
		memcpy(data->cigar,o->cigar,o->cigarspace);
		data->cigarspace = o->cigarspace;
	}
	
	if ( o->auxbuffer )
	{
		data->auxbuffer = BamBam_CharBuffer_Clone(o->auxbuffer);
		if ( ! data->auxbuffer )
			return BamBam_BamSingleAlignment_Delete(data);
	}
		
	return data;
}
