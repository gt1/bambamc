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

#include <bambamc/BamBam_Chromosome.h>
#include <stdlib.h>
#include <string.h>

BamBam_Chromosome * BamBam_Chromosome_New(char const * rname, uint64_t const rlength)
{
	BamBam_Chromosome * chr = (BamBam_Chromosome *)malloc(sizeof(BamBam_Chromosome));
	
	if ( ! chr )
		return 0;

	chr->name = strdup(rname);
	
	if ( ! chr->name )
	{
		free(chr);
		return 0;
	}
	
	chr->length = rlength;

	return chr;
}

void BamBam_Chromosome_Delete(BamBam_Chromosome * chr)
{
	if ( chr )
	{
		free(chr->name);
		free(chr);
	}
}
