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

int main()
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
	
	#if 0	
	fprintf(stderr,"Filtered:\n%s",col->filteredbamheadertext);
	fprintf(stderr,"Head:\n%s",col->bamheadertext);
	fprintf(stderr,"Number of sequences:%d\n",col->bamheader->n_targets);
	#endif

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
