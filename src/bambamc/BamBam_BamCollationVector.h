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

#if ! defined(BAMBAM_BAMCOLLATIONVECTOR_H)
#define BAMBAM_BAMCOLLATIONVECTOR_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_BamCollationHashEntry.h>
#include <bambamc/BamBam_BamCollationOutputVector.h>
#include <bambamc/BamBam_BamCollationTempFileGenerator.h>

typedef struct _BamBam_BamCollationVector
{
	BamBam_BamCollationHashEntry ** entries;
	uint64_t tablesize;
	uint64_t fill;
} BamBam_BamCollationVector;

extern BamBam_BamCollationVector * BamBam_BamCollationVector_New(unsigned int sizelog) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_BamCollationVector_PushBack(BamBam_BamCollationVector * vector, BamBam_BamCollationHashEntry * entry) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollationVector_Erase(BamBam_BamCollationVector * vector);
extern int BamBam_BamCollationVector_Sort(
	BamBam_BamCollationVector * vector,
	BamBam_BamCollationOutputVector * outputvector,
	BamBam_BamCollationTempFileGenerator * gen,
	bam_header_t * bamheader
) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollationVector_Delete(BamBam_BamCollationVector * vector);
#endif
