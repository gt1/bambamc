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

#include <bambamc/BamBam_BamHeaderInfo.h>
#include <bambamc/BamBam_BamWriter.h>
#include <bambamc/BamBam_BamAlignmentPut.h>
#include <assert.h>

int testLibBamFree(FILE * file)
{
	int r = -1;
	BamBam_BamHeaderInfo * hi = 0;
	BamBam_BgzfCompressor * bgzf = 0;
	BamBam_AlignmentPut * bap = 0;
	
	hi = BamBam_BamHeaderInfo_New("1.4","unknown",0);
	assert ( hi );
	r = BamBam_BamHeaderInfo_AddChromosome(hi, "chr1",10000);
	assert ( ! r );

	bgzf = BamBam_BgzfCompressor_NewFP(file,1);
	assert ( bgzf );
	
	r = BamBam_BamHeaderInfo_WriteBamHeader(hi,bgzf);
	assert ( r >= 0 );
	
	bap = BamBam_AlignmentPut_New();
	assert ( bap );

	r = BamBam_CharBuffer_PutAlignmentC(
		bap,
		0,
		0,
		5000,
		-1,
		0,
		"readname",
		"ACGTTGCA",
		"HHHHHHHH",
		"8M",
		60,
		100);		
	assert ( r >= 0 );
	int val = 61;
	r = BamBam_CharBuffer_PutAuxNumberC(bap,"AS",'i',&val);
	assert ( ! r );
	int val2 = 5;
	r = BamBam_CharBuffer_PutAuxNumberC(bap,"NM",'i',&val2);
	assert ( ! r );
	
	r = BamBam_BamSingleAlignment_StoreAlignmentBgzf(bap->calignment,bgzf);
	assert ( r >= 0 );
	
	r = BamBam_BgzfCompressor_Terminate(bgzf);
	assert ( r >= 0 );
	
	BamBam_BgzfCompressor_Delete(bgzf);
	BamBam_AlignmentPut_Delete(bap);
	BamBam_BamHeaderInfo_Delete(hi);
	
	fflush(file);
	
	return 0;
}

#if ! defined(BAMBAMC_BAMONLY)
int testLibBamBased()
{
	BamBam_BamHeaderInfo * hi = 0;
	BamBam_BamWriter * wr = 0;
	int r = -1;

	hi = BamBam_BamHeaderInfo_New("1.4","unknown",0);
	assert ( hi );
	r = BamBam_BamHeaderInfo_AddChromosome(hi, "chr1",10000);
	assert ( ! r );

	wr = BamBam_BamWriter_New(hi,"-",1);
	assert ( wr );

	r = BamBam_CharBuffer_PutAlignment(
		wr->aput,
		0,
		0,
		5000,
		-1,
		0,
		"readname",
		"ACGTTGCA",
		"HHHHHHHH",
		"8M",
		60,
		100);		
	assert ( ! r );
	int val = 61;
	r = BamBam_CharBuffer_PutAuxNumber(wr->aput,"AS",'i',&val);
	assert ( ! r );
	int val2 = 5;
	r = BamBam_CharBuffer_PutAuxNumber(wr->aput,"NM",'i',&val2);
	assert ( ! r );

	bam_write1(wr->outfile,wr->aput->alignment);
	
	BamBam_BamWriter_Delete(wr);
	BamBam_BamHeaderInfo_Delete(hi);
	
	return 0;
}
#endif

int main()
{
	#if defined(BAMBAMC_BAMONLY)
	return testLibBamFree(stdout);
	#else
	return testLibBamBased();
	#endif

	return 0;
}
