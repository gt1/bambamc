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

#include <bambamc/BamBam_FormatAlignment.h>
#include <bambamc/BamBam_BamSingleAlignment.h>
#include <bambamc/BamBam_BamFlagBase.h>
#include <ctype.h>

static uint64_t BamBam_GetFastqNameLineLength(unsigned int const qnamelen, uint32_t const flags)
{
	return 1 /* @ */ + qnamelen + (( flags & BAMBAMC_FPAIRED ) ? 2 : 0) /* /[12] */ + 1 /* \n */;
}
static uint64_t BamBam_GetFastqSeqLineLength(unsigned int const seqlen)
{
	return seqlen + 1;
}
static uint64_t BamBam_GetFastqPlusLineLength()
{
	return 1 /* + */ + 1 /* \n */;
}
static uint64_t BamBam_GetFastqQualLineLength(unsigned int const seqlen)
{
	return seqlen + 1;
}

static uint64_t BamBam_GetFastqEntryLength(unsigned int const qnamelen, unsigned int const seqlen, uint32_t const flags)
{
	return
		BamBam_GetFastqNameLineLength(qnamelen,flags) +
		BamBam_GetFastqSeqLineLength(seqlen) +
		BamBam_GetFastqPlusLineLength(qnamelen) +
		BamBam_GetFastqQualLineLength(seqlen);
}
/**
 * put fastq @ line
 **/
static char * BamBam_PutAtLine(
	char const * qname, 
	unsigned int const qnamelen, 
	uint32_t const flags, 
	char * opc, 
	char const term
)
{
	static int spaceTable[256];
	static int spaceTableInitialized = 0;
	
	char const * qnamee = qname+qnamelen;
	
	if ( ! spaceTableInitialized )
	{
		int i;
		for ( i = 0; i < 256; ++i )
			spaceTable[i] = isspace(i);
		spaceTableInitialized = 1;
	}
			
	*(opc++) = '@';
	
	/* paired? add /1 or /2 before first space or at end of line */
	if ( flags & BAMBAMC_FPAIRED )
	{
		while ( qname != qnamee && !spaceTable[(int)((uint8_t)(*qname))] )
			*(opc++) = *(qname++);

		*(opc++) = '/';
		if ( flags & BAMBAMC_FREAD1 )
			*(opc++) = '1';
		else
			*(opc++) = '2';

		while ( qname != qnamee )
			*(opc++) = *(qname++);
	}
	else
	{
		while ( qname != qnamee )
			*(opc++) = *(qname++);
	}

	
	*(opc++) = term;
	
	return opc;
}
static char * BamBam_PutPlusLine(char * opc, char const term)
{
	*opc++ = '+';
	*opc++ = term;
	return opc;
}
static char * BamBam_PutAlignmentFastQ(BamBam_BamSingleAlignment const * alignment, char * opc, char const term)
{
	char const * qname = BamBam_BamSingleAlignment_GetReadName(alignment);
	uint32_t const flags = BamBam_BamSingleAlignment_GetFlags(alignment);
	int32_t const seqlen = BamBam_BamSingleAlignment_GetLSeq(alignment);

	/* @ line */
	opc = BamBam_PutAtLine(qname,strlen(qname),flags,opc,term);
	/* seq line */
	memcpy(opc,alignment->query,seqlen); opc += seqlen; *(opc++) = term;
	/* + line */
	opc = BamBam_PutPlusLine(opc,term);
	/* quality */
	memcpy(opc,alignment->qual,seqlen); opc += seqlen; *(opc++) = term;

	return opc;
}
static uint64_t BamBam_GetFastQAlignmentLength(BamBam_BamSingleAlignment const * alignment)
{
	int32_t const seqlen = BamBam_BamSingleAlignment_GetLSeq(alignment);
	uint32_t const flags = BamBam_BamSingleAlignment_GetFlags(alignment);
	char const * qname = BamBam_BamSingleAlignment_GetReadName(alignment);
	unsigned int const qnamelen = strlen(qname);
	return BamBam_GetFastqEntryLength(qnamelen, seqlen, flags);
}
int BamBam_PutAlignmentFastQBuffer(
	BamBam_BamSingleAlignment * alignment, 
	char ** buffer, 
	unsigned int * bufferlen, 
	char const term
)
{
	unsigned int const neededlength = BamBam_GetFastQAlignmentLength(alignment);
	char * endptr = 0;

	if ( neededlength > *bufferlen )
	{
		free(*buffer);
		*buffer = 0;
		*bufferlen = 0;
		
		*buffer = (char *)malloc(neededlength);
		
		if ( ! *buffer )
			return -1;
		
		*bufferlen = neededlength;
	}
	
	if ( BamBam_BamSingleAlignment_DecodeQuery(alignment) < 0 )
		return -1;
	if ( BamBam_BamSingleAlignment_DecodeQual(alignment) < 0 )
		return -1;
	
	endptr = BamBam_PutAlignmentFastQ(alignment,*buffer,term);
	
	return endptr - *buffer;
}

/*
 * samtools/libbam dependent code
 */
static char * BamBam_PutQuality(uint8_t const * qual, unsigned int const seqlen, uint32_t const flags, char * opc, char const term)
{
	if ( seqlen )
	{
		if ( qual[0] == 0xFF )
		{
			unsigned int i;
			for ( i = 0; i < seqlen; ++i )				
				*(opc++) = '*';
		}
		else
		{
			if ( flags & BAMBAMC_FREVERSE )
			{
				uint8_t const * qualp = qual + seqlen;
				
				while ( qualp != qual )
					*(opc++) = (*(--qualp)) + 33;
			}
			else
			{
				uint8_t const * const quale = qual+seqlen;

				while ( qual != quale )
					*(opc++) = *(qual++) + 33;
			}
		}
	}
	*(opc++) = term;
	
	return opc;
}
/**
 * decode a query string in compacted BAM form to clear text
 **/
static char * BamBam_PutQuery(uint8_t const * seq, unsigned int const seqlen, uint32_t const flags, char * opc, char const term)
{
	#if 0
	/* character mapping table */
	static uint8_t const T[16] = { 
		4 /* 0 */, 0 /* 1 */, 1 /* 2 */, 4 /* 3 */,
		2 /* 4 */, 4 /* 5 */, 4 /* 6 */, 4 /* 7 */,
		3 /* 8 */, 4 /* 9 */, 4 /* 10 */, 4 /* 11 */,
		4 /* 12 */, 4 /* 13 */, 4 /* 14 */, 4 /* 15 */
	};
	/* reverse complement */
	static uint8_t const I[5] = { 3, 2, 1, 0, 4 };
	/* remap */
	static uint8_t const R[5] = { 'A', 'C', 'G', 'T', 'N' };
	#endif
	
	/* character mapping table */
	static uint8_t const F[16] = { 
		'N' /* R[T[0]] */,'A' /* R[T[1]] */,'C' /* R[T[2]] */,
		'N' /* R[T[3]] */,'G' /* R[T[4]] */,'N' /* R[T[5]] */,
		'N' /* R[T[6]] */,'N' /* R[T[7]] */,'T' /* R[T[8]] */,
		'N' /* R[T[9]] */,'N' /* R[T[10]] */,'N' /* R[T[11]] */,
		'N' /* R[T[12]] */,'N' /* R[T[13]] */,'N' /* R[T[14]] */,'N' /* R[T[15]] */ 
	};
	static uint8_t const C[16] = { 
		'N' /* R[I[T[0]]] */,'T' /* R[I[T[1]]] */,'G' /* R[I[T[2]]] */,
		'N' /* R[I[T[3]]] */,'C' /* R[I[T[4]]] */,'N' /* R[I[T[5]]] */,
		'N' /* R[I[T[6]]] */,'N' /* R[I[T[7]]] */,'A' /* R[I[T[8]]] */,
		'N' /* R[I[T[9]]] */,'N' /* R[I[T[10]]] */,'N' /* R[I[T[11]]] */,
		'N' /* R[I[T[12]]] */,'N' /* R[I[T[13]]] */,'N' /* R[I[T[14]]] */,
		'N' /* R[I[T[15]]] */ 
	};

	/* reverse complement? */
	if ( flags & BAMBAMC_FREVERSE )
	{
		unsigned int i = seqlen;
		while ( i-- )
			*(opc++) = C[bam1_seqi(seq,i)];
	}
	else
	{
		unsigned int i;
		for ( i = 0; i < seqlen; ++i )
			*(opc++) = F[bam1_seqi(seq,i)];
	}

	/* newline */
	*(opc++) = term;
	
	return opc;
}
static char * BamBam_PutRead(
	char const * qname,
	unsigned int const qnamelen,
	uint8_t const * seq,
	uint8_t const * qual, 
	unsigned int const seqlen, 
	uint32_t const flags, 
	char * opc,
	char const term
	)
{
	/* at line */
	opc = BamBam_PutAtLine(qname,qnamelen,flags,opc,term);
	/* sequence */
	opc = BamBam_PutQuery(seq,seqlen,flags,opc,term);
	/* plus line */
	opc = BamBam_PutPlusLine(opc,term);
	/* quality */
	opc = BamBam_PutQuality(qual,seqlen,flags,opc,term);
	
	return opc;
}
static char * BamBam_PutAlignment(bam1_t const * alignment, char * opc, char const term)
{
	bam1_core_t const * alignment_core = &(alignment->core);
	/* get query string */
	uint8_t const * seq = bam1_seq(alignment);
	/* length of query string */
	unsigned int const seqlen = alignment_core->l_qseq;
	/* flags */
	uint32_t const flags = alignment_core->flag;
	/* name of query */
	char const * qname = bam1_qname(alignment);
	unsigned int const qnamelen = strlen(qname);
	
	/* put reads in buffer */
	return BamBam_PutRead(qname,qnamelen,seq,bam1_qual(alignment),seqlen,flags,opc,term);
}
static uint64_t BamBam_GetAlignmentLength(bam1_t const * alignment)
{
	bam1_core_t const * alignment_core = &(alignment->core);
	/* length of query string */
	unsigned int const seqlen = alignment_core->l_qseq;
	/* flags */
	uint32_t const flags = alignment_core->flag;
	/* name of query */
	char const * qname = bam1_qname(alignment);
	unsigned int const qnamelen = strlen(qname);
	
	return BamBam_GetFastqEntryLength(qnamelen, seqlen, flags);
}
int BamBam_PutAlignmentBuffer(bam1_t const * alignment, char ** buffer, unsigned int * bufferlen, char const term)
{
	unsigned int const neededlength = BamBam_GetAlignmentLength(alignment);
	char * endptr = 0;

	if ( neededlength > *bufferlen )
	{
		free(*buffer);
		*buffer = 0;
		*bufferlen = 0;
		
		*buffer = (char *)malloc(neededlength);
		
		if ( ! *buffer )
			return -1;
		
		*bufferlen = neededlength;
	}
	
	endptr = BamBam_PutAlignment(alignment,*buffer,term);
	
	return endptr - *buffer;
}
