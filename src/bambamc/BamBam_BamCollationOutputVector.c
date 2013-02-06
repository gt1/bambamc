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

#include <bambamc/BamBam_BamCollationOutputVector.h>
#include <assert.h>

void BamBam_BamCollationOutputVector_PushFront(BamBam_BamCollationOutputVector * outputvector, BamBam_BamCollationHashEntry * entry)
{
	assert (outputvector->outputvectorfill < outputvector->outputvectorsize );
	if ( outputvector->outputvectorlow == 0 )
	{
		outputvector->outputvectorlow  += outputvector->outputvectorsize;
		outputvector->outputvectorhigh += outputvector->outputvectorsize;		
	}
	outputvector->outputvector[(--outputvector->outputvectorlow) % outputvector->outputvectorsize] =
		entry;
	outputvector->outputvectorfill++;
}

void BamBam_BamCollationOutputVector_PushBack(BamBam_BamCollationOutputVector * outputvector, BamBam_BamCollationHashEntry * entry)
{
	assert (outputvector->outputvectorfill < outputvector->outputvectorsize );
	outputvector->outputvector[(outputvector->outputvectorhigh++) % outputvector->outputvectorsize] =
		entry;
	outputvector->outputvectorfill++;
}

BamBam_BamCollationHashEntry * BamBam_BamCollationOutputVector_PopFront(BamBam_BamCollationOutputVector * outputvector)
{
	assert ( outputvector->outputvectorfill );
	BamBam_BamCollationHashEntry * entry = outputvector->outputvector[(outputvector->outputvectorlow++) % outputvector->outputvectorsize];
	outputvector->outputvectorfill--;
	return entry;
}

BamBam_BamCollationOutputVector * BamBam_BamCollationOutputVector_New(unsigned int const vectorsizelog)
{
	BamBam_BamCollationOutputVector * outputvector = 0;
	
	outputvector = (BamBam_BamCollationOutputVector *)malloc(sizeof(BamBam_BamCollationOutputVector));
	
	if ( ! outputvector )
		return 0;

	outputvector->outputvectorsize = 1ull<<vectorsizelog;
	outputvector->outputvectorlow = 0;
	outputvector->outputvectorhigh = 0;
	outputvector->outputvectorfill = 0;
	outputvector->outputvector = (BamBam_BamCollationHashEntry **)malloc((outputvector->outputvectorsize)*sizeof(BamBam_BamCollationHashEntry *));
	
	return outputvector;
}
void BamBam_BamCollationOutputVector_Delete(BamBam_BamCollationOutputVector * outputvector)
{
	uint64_t i;
	
	for ( i = outputvector->outputvectorlow; i != outputvector->outputvectorhigh; ++i )
	{
		if ( outputvector->outputvector[i % outputvector->outputvectorsize] )
		{
			BamBam_BamCollationHashEntry_Delete(outputvector->outputvector[i%outputvector->outputvectorsize]);
			outputvector->outputvector[i%outputvector->outputvectorsize] = 0;
		}
	}
	
	free(outputvector->outputvector);
	free(outputvector);
}
