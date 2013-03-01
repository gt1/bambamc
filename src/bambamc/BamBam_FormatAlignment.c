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

