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
#include <assert.h>

int main()
{
	int r = -1;
	BamBam_BamHeaderInfo * hi = 0;
	BamBam_BamWriter * wr = 0;
	
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
	bam_write1(wr->outfile,wr->aput->alignment);
	
	BamBam_BamWriter_Delete(wr);
	BamBam_BamHeaderInfo_Delete(hi);

	return 0;
}
