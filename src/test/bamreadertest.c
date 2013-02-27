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
#include <bambamc/BamBam_BamCollator.h>
#include <bambamc/BamBam_FormatAlignment.h>
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
	col = BamBam_BamCollator_New("tmpdir",16,16,"bam","-");
	
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
			// fprintf(stdout,"%s\t%s\n", entryA->qname, entryB->qname);

			aok = BamBam_PutAlignmentBuffer(entryA->entry,&bufferA,&bufferAlen,'\n');
			bok = BamBam_PutAlignmentBuffer(entryB->entry,&bufferB,&bufferBlen,'\n');
			
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

#include <bambamc/BamBam_BamFileDecoder.h>

int main()
{
	char const * inputfilename = "/popper/scratch01/assembly/gt1/bamtofastqtest/name.bam";
	BamBam_BamFileDecoder * decoder = 0;
	BamBam_BamSingleAlignment * algn = 0;
	samfile_t * bamfile = 0;
	bam_header_t * bamheader = 0;
	char * buffer = 0;
	unsigned int bufferlen = 0;
	                
	bamfile = samopen(inputfilename,"rb",0);
	
	if ( ! bamfile )
	{
		return EXIT_FAILURE;
	}
	
	bamheader = bamfile->header;

	if ( ! bamheader )
	{
		return EXIT_FAILURE;
	}
	
	decoder = BamBam_BamFileDecoder_New(inputfilename);

	if ( ! decoder )
	{
		samclose(bamfile);
		return EXIT_FAILURE;
	}

	while ( (algn =  BamBam_BamFileDecoder_DecodeAlignment(decoder)) )
	{
	
		int const ok = BamBam_PutAlignmentFastQBuffer(algn, &buffer, &bufferlen, '\n');

		if ( ok < 0 )
			break;
		
		fwrite(buffer,ok,1,stdout);
	}
	
	BamBam_BamFileDecoder_Delete(decoder);
	samclose(bamfile);
	
	return EXIT_SUCCESS;
}
