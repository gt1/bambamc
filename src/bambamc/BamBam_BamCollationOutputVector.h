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

#if ! defined(BAMBAM_BAMCOLLATIONOUTPUTVECTOR_H)
#define BAMBAM_BAMCOLLATIONOUTPUTVECTOR_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_BamCollationHashEntry.h>

typedef struct _BamBam_BamCollationOutputVector
{
	BamBam_BamCollationHashEntry ** outputvector;
	uint64_t outputvectorsize;
	uint64_t outputvectorfill;
	uint64_t outputvectorlow;
	uint64_t outputvectorhigh;
} BamBam_BamCollationOutputVector;

extern void BamBam_BamCollationOutputVector_PushBack(BamBam_BamCollationOutputVector * outputvector, BamBam_BamCollationHashEntry * entry);
extern void BamBam_BamCollationOutputVector_PushFront(BamBam_BamCollationOutputVector * outputvector, BamBam_BamCollationHashEntry * entry);
extern BamBam_BamCollationHashEntry * BamBam_BamCollationOutputVector_PopFront(BamBam_BamCollationOutputVector * outputvector) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BamCollationOutputVector * BamBam_BamCollationOutputVector_New(unsigned int const vectorsizelog) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollationOutputVector_Delete(BamBam_BamCollationOutputVector * outputvector);
#endif
