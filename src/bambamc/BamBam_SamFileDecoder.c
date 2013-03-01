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

#include <bambamc/BamBam_SamFileDecoder.h>

BamBam_BamSingleAlignment * BamBam_SamFileDecoder_LoadAlignment(BamBam_SamFileDecoder * object)
{
	char const * pa = 0;
	char const * pe = 0;
	char const * p = 0;
	unsigned int col = 0;
	int r = 0;
	
	if ( BamBam_LineBuffer_GetLine(object->lb,&pa,&pe) )
		return 0;

	assert ( *pe == '\n' );
		
	p = pa;
	while ( p != pe && col < BAMBAMC_SAMFILEDECODER_MANDATORYCOLUMNS )
	{
		char const * pc = p;
		while ( p != pe && *p != '\t' )
			++p;
	
		object->fields[col][0] = pc;
		object->fields[col][1] = p;
		
		// skip over tab
		if ( p != pe )
			++p;
		col += 1;
	}
	
	if ( col != BAMBAMC_SAMFILEDECODER_MANDATORYCOLUMNS || BamBam_SamInfo_parseSamLine(&object->fields[0],object->saminfo) < 0 )
	{
		fprintf(stderr, "Defect SAM line: ");
		fwrite(pa,pe-pa,1,stderr);
		fprintf(stderr,"\n");	
		
		return 0;
	}

	r = BamBam_CharBuffer_PutAlignmentC(
		object->aput,
		object->saminfo->flag,
		BamBam_BamFileHeader_FindChromosomeIdByName(object->header,object->saminfo->rname),
		object->saminfo->pos-1,
		BamBam_BamFileHeader_FindChromosomeIdByName(object->header,object->saminfo->rnext),
		object->saminfo->pnext-1,
		object->saminfo->qname,
		object->saminfo->seq,
		object->saminfo->qual,
		object->saminfo->cigar,
		object->saminfo->mapq,
		object->saminfo->tlen
	);
	
	if ( r < 0 )
		return 0;

	#if 0
	fprintf(stderr, "ok: ");
	fwrite(pa,pe-pa,1,stderr);
	fprintf(stderr,"\n");			
	#endif			
	
	return object->aput->calignment;
}

BamBam_SamFileDecoder * BamBam_SamFileDecoder_Delete(BamBam_SamFileDecoder * object)
{
	if ( object )
	{
		if ( object->closefile )
		{
			fclose(object->closefile);
		}
		if ( object->header )
		{
			BamBam_BamFileHeader_Delete(object->header);
		}
		if ( object->lb )
		{
			BamBam_LineBuffer_Delete(object->lb);
		}
		if ( object->saminfo )
		{
			BamBam_SamInfo_Delete(object->saminfo);
		}
		if ( object->aput )
		{
			BamBam_AlignmentPut_Delete(object->aput);
		}
		free(object);
	}
	
	return 0;
}

BamBam_SamFileDecoder * BamBam_SamFileDecoder_NewInternal(BamBam_SamFileDecoder * object, FILE * file)
{
	BamBam_SamInfo_initSamCharTables();
	
	object->header = BamBam_BamFileHeader_New_SAM(file);
	
	if ( ! object->header )
		return BamBam_SamFileDecoder_Delete(object);

	object->lb = BamBam_LineBuffer_New(file,1024);
	
	if ( ! object->lb )
		return BamBam_SamFileDecoder_Delete(object);

	object->saminfo = BamBam_SamInfo_New();
	
	if ( ! object->saminfo )
		return BamBam_SamFileDecoder_Delete(object);

	object->aput = BamBam_AlignmentPut_New();
	
	if ( ! object->aput )
		return BamBam_SamFileDecoder_Delete(object);
	
	return object;
}

BamBam_SamFileDecoder * BamBam_SamFileDecoder_NewFilename(char const * filename)
{
	BamBam_SamFileDecoder * object = 0;
	
	object = (BamBam_SamFileDecoder *)malloc(sizeof(BamBam_SamFileDecoder));
	
	if ( ! object )
		return BamBam_SamFileDecoder_Delete(object);
		
	memset(object,0,sizeof(BamBam_SamFileDecoder));
	
	object->closefile = fopen(filename,"rb");
	
	if ( ! object->closefile )
		return BamBam_SamFileDecoder_Delete(object);
	
	return BamBam_SamFileDecoder_NewInternal(object,object->closefile);
}

BamBam_SamFileDecoder * BamBam_SamFileDecoder_NewFd(FILE * file)
{
	BamBam_SamFileDecoder * object = 0;
	
	object = (BamBam_SamFileDecoder *)malloc(sizeof(BamBam_SamFileDecoder));
	
	if ( ! object )
		return BamBam_SamFileDecoder_Delete(object);
		
	memset(object,0,sizeof(BamBam_SamFileDecoder));

	return BamBam_SamFileDecoder_NewInternal(object,file);		
}

BamBam_SamFileDecoder * BamBam_SamFileDecoder_New(char const * filename)
{
	if ( strcmp(filename,"-") == 0 )
		return BamBam_SamFileDecoder_NewFd(stdin);
	else
		return BamBam_SamFileDecoder_NewFilename(filename);
}
