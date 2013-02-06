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
#if ! defined(BAMBAM_BAMFLAGS)
#define BAMBAM_BAMFLAGS

enum bam_flags
{
	BAMBAM_FPAIRED = (1u << 0),
	BAMBAM_FPROPER = (1u << 1),
	BAMBAM_FUNMAP = (1u << 2),
	BAMBAM_FMUNMAP = (1u << 3),
	BAMBAM_FREVERSE = (1u << 4),
	BAMBAM_FMREVERSE = (1u << 5),
	BAMBAM_FREAD1 = (1u << 6),
	BAMBAM_FREAD2 = (1u << 7),
	BAMBAM_FSECONDARY = (1u << 8),
	BAMBAM_FQCFAIL = (1u << 9),
	BAMBAM_FDUP = (1u << 10)
};
#endif
