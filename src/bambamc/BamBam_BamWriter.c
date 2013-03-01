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

#include <bambamc/BamBam_BamWriter.h>
#include <bambamc/BamBam_BamAlignmentPut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

BamBam_BamWriter * BamBam_BamWriter_Delete(BamBam_BamWriter * writer, int * termstatus)
{
	if ( writer )
	{
		if ( writer->aput )
		{
			BamBam_AlignmentPut_Delete(writer->aput);
			writer->aput = 0;
		}
		if ( writer->bgzf )
		{
			int const r = BamBam_BgzfCompressor_Terminate(writer->bgzf);
			if ( termstatus )
				*termstatus = r;
			BamBam_BgzfCompressor_Delete(writer->bgzf);
			writer->bgzf = 0;
		}

		free( writer );
	}
	
	return 0;
}

BamBam_BamWriter * BamBam_BamWriter_New(
	BamBam_BamHeaderInfo * info,
	char const * filename,
	int compressionLevel)
{
	BamBam_BamWriter * writer = (BamBam_BamWriter *)malloc(sizeof(BamBam_BamWriter));
	char mode[3] = "w1\0";
	
	assert ( mode[2] == 0 );

	if ( !writer )
		return BamBam_BamWriter_Delete(writer,0);
		
	memset(writer,0,sizeof(BamBam_BamWriter));

	if ( compressionLevel < 0 || compressionLevel > 9 )
		return BamBam_BamWriter_Delete(writer,0);
		
	mode[1] = compressionLevel + '0';
	
	writer->bgzf = BamBam_BgzfCompressor_New(filename,compressionLevel);

	if ( BamBam_BamHeaderInfo_WriteBamHeader(info,writer->bgzf) < 0 )
		return BamBam_BamWriter_Delete(writer,0);	

	writer->aput = BamBam_AlignmentPut_New();

	if ( ! writer->aput )
		return BamBam_BamWriter_Delete(writer,0);

	return writer;
}
int BamBam_BamWriter_PutAlignment(
	BamBam_BamWriter * writer,
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
	return BamBam_CharBuffer_PutAlignmentC(writer->aput,flags,tid,rpos,mtid,rmpos,name,query,qual,cigar,rqual,isize);
}
int BamBam_BamWriter_PutAuxNumber(BamBam_BamWriter * writer, char const * tag, char const type, void const * rvalue)
{
	return BamBam_CharBuffer_PutAuxNumberC(writer->aput,tag,type,rvalue);
}
int BamBam_BamWriter_Commit(BamBam_BamWriter * writer)
{
	return BamBam_BamSingleAlignment_StoreAlignmentBgzf(writer->aput->calignment,writer->bgzf);
}
