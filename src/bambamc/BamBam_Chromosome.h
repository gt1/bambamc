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

#if ! defined(BAMBAM_CHROMOSOME_H)
#define BAMBAM_CHROMOSOME_H

#include <bambamc/BamBam_Unused.h>
#include <stdint.h>

typedef struct _BamBam_Chromosome
{
	char * name;
	uint64_t length;
} BamBam_Chromosome;

BamBam_Chromosome * BamBam_Chromosome_New(char const * rname, uint64_t const rlength) BAMBAM_WARN_IF_UNUSEDRESULT;
void BamBam_Chromosome_Delete(BamBam_Chromosome * chr);
#endif
