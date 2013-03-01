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
#include <bambamc/BamBam_LineBuffer.h>
#include <bambamc/BamBam_BamCollator.h>
#include <bambamc/BamBam_FormatAlignment.h>
#include <bambamc/BamBam_SamBamFileDecoder.h>
#include <assert.h>

int runCollationTest()
{
	BamBam_BamCollator * col = 0;
	int cnt = 0;
	BamBam_BamCollationHashEntry * entryA = 0;
	BamBam_BamCollationHashEntry * entryB = 0;
	char * bufferA = 0, * bufferB = 0;
	unsigned int bufferAlen = 0, bufferBlen = 0;
	int aok, bok;
	
	/* allocate collator */
	col = BamBam_BamCollator_New("tmpdir",16,16,"sam","-");
	
	if ( ! col )
	{
		fprintf(stderr,"Failed to open bam file.\n");
		return EXIT_FAILURE;
	}
	
	/* get pairs */
	while ( (cnt = BamBam_BamCollator_Get(col,&entryA,&entryB)) )
	{
		if ( cnt == 2 )
		{
			assert ( entryA );
			assert ( entryB );
			/* fprintf(stdout,"%s\t%s\n", entryA->qname, entryB->qname); */

			aok = BamBam_PutAlignmentFastQBuffer(entryA->entry,&bufferA,&bufferAlen,'\n');
			bok = BamBam_PutAlignmentFastQBuffer(entryB->entry,&bufferB,&bufferBlen,'\n');
			
			if ( aok >= 0 && bok >= 0 )
			{
				fwrite(bufferA,aok,1,stdout);
				fwrite(bufferB,bok,1,stdout);
			}
		}	
		
		BamBam_BamCollationHashEntry_Delete(entryA);
		BamBam_BamCollationHashEntry_Delete(entryB);
	}
	
	free(bufferA);
	free(bufferB);
	BamBam_BamCollator_Delete(col);
	
	return 0;
}

void lineBufferTest()
{
	char const * pa = 0;
	char const * pe = 0;
	unsigned int numlines = 0;
	BamBam_LineBuffer * lb = BamBam_LineBuffer_New(stdin,1024);
	assert ( lb );
	
	while ( ! BamBam_LineBuffer_GetLine(lb,&pa,&pe) )
	{
		if ( pe != pa && pa[0] == '@' )
		{
			fprintf(stderr,"Header line: ");
			fwrite(pa,pe-pa,1,stderr);
			fprintf(stderr,"\n");
		}
		else
		{
			BamBam_LineBuffer_PutBack(lb,pa);
			break;
		}
	}
	while ( ! BamBam_LineBuffer_GetLine(lb,&pa,&pe) )
	{
		fprintf(stderr,"Non header line: ");
		fwrite(pa,pe-pa,1,stderr);
		fprintf(stderr,"\n");
	}
	
	BamBam_LineBuffer_Delete(lb);
	
	fprintf(stderr,"number of lines is %u\n", numlines);

}

void samBamSamTest()
{
	BamBam_SamBamFileDecoder * samdec = BamBam_SamBamFileDecoder_New("-","r");
	BamBam_BamSingleAlignment * algn = 0;
	assert ( samdec );

	while ( (algn = BamBam_SamBamFileDecoder_DecodeAlignment(samdec)) )
	{
		char const * name = BamBam_BamSingleAlignment_GetReadName(algn);
		fprintf(stderr,"%s\n", name);
	}
	
	BamBam_SamBamFileDecoder_Delete(samdec);
}

int main(int argc, char * argv[])
{
	runCollationTest();	
	/* lineBufferTest(); */
		
	return EXIT_SUCCESS;
}
