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

#if ! defined(BAMBAM_BAMCOLLATIONHASH_H)
#define BAMBAM_BAMCOLLATIONHASH_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_BamCollationHashEntry.h>

typedef struct _BamBam_BamCollationHash
{
	BamBam_BamCollationHashEntry ** entries;
	uint64_t tablesize;
	uint64_t tablemask;
} BamBam_BamCollationHash;

extern BamBam_BamCollationHash * BamBam_BamCollationHash_New(unsigned int sizelog) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollationHash_Delete(BamBam_BamCollationHash * hash);
#endif
