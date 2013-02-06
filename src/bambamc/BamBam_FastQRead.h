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

#if ! defined(FASTQREAD_H)
#define FASTQREAD_H

enum alignment_type { BAMBAM_ALIGNMENT_TYPE_UNDEFINED = -2, BAMBAM_ALIGNMENT_TYPE_NONE = -1, BAMBAM_ALIGNMENT_TYPE_SINGLE = 1, BAMBAM_ALIGNMENT_TYPE_COMPLETE_PAIR = 2, BAMBAM_ALIGNMENT_TYPE_ORPHAN1_PAIR = 3, BAMBAM_ALIGNMENT_TYPE_ORPHAN2_PAIR = 4 };

typedef struct _BamBam_FastQRead
{
	char const * name;
	unsigned int namelength;
	char const * seq;
	unsigned int seqlength;
	char const * qual;
} BamBam_FastQRead;
#endif
