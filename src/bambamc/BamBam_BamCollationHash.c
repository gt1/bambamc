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

#include <bambamc/BamBam_BamCollationHash.h>

void BamBam_BamCollationHash_Delete(BamBam_BamCollationHash * hash)
{
	if ( hash )
	{
		free(hash->entries);
		free(hash);
	}
}

BamBam_BamCollationHash * BamBam_BamCollationHash_New(unsigned int sizelog)
{
	BamBam_BamCollationHash * hash = 0;
	uint64_t i;
	
	hash = (BamBam_BamCollationHash *)malloc(sizeof(BamBam_BamCollationHash));

	if ( ! hash )
		return 0;
		
	hash->entries = 0;
	hash->tablesize = 1ull << sizelog;
	hash->tablemask = hash->tablesize-1;
	
	hash->entries = (BamBam_BamCollationHashEntry **)malloc(hash->tablesize*(sizeof(BamBam_BamCollationHashEntry *)));
	
	if ( ! hash->entries )
	{
		free(hash);
		return 0;
	}
	
	for ( i = 0; i < hash->tablesize; ++i )
		hash->entries[i] = 0;
	
	return hash;
}
