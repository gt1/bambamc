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

BamBam_BamWriter * BamBam_BamWriter_Delete(BamBam_BamWriter * writer)
{
	if ( writer )
	{
		BamBam_AlignmentPut_Delete(writer->aput);
		writer->aput = 0;
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
	int headerok = -1;
	
	assert ( mode[2] == 0 );

	if ( !writer )
		return BamBam_BamWriter_Delete(writer);
		
	memset(writer,0,sizeof(BamBam_BamWriter));

	if ( compressionLevel < 1 || compressionLevel > 9 )
		return BamBam_BamWriter_Delete(writer);
		
	mode[1] = compressionLevel + '0';

	headerok = BamBam_BamHeaderInfo_ProduceHeaderText(info);

	if ( headerok < 0 )
		return BamBam_BamWriter_Delete(writer);

	writer->aput = BamBam_AlignmentPut_New();

	if ( ! writer->aput )
		return BamBam_BamWriter_Delete(writer);

	return writer;
}
