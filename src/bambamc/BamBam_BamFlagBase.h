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
	BAMBAMC_FPAIRED = (1u << 0),
	BAMBAMC_FPROPER_PAIR = (1u << 1),
	BAMBAMC_FUNMAP = (1u << 2),
	BAMBAMC_FMUNMAP = (1u << 3),
	BAMBAMC_FREVERSE = (1u << 4),
	BAMBAMC_FMREVERSE = (1u << 5),
	BAMBAMC_FREAD1 = (1u << 6),
	BAMBAMC_FREAD2 = (1u << 7),
	BAMBAMC_FSECONDARY = (1u << 8),
	BAMBAMC_FQCFAIL = (1u << 9),
	BAMBAMC_FDUP = (1u << 10)
};

enum bambam_cigar_operators
{
	BAMBAMC_CMATCH = 0,
	BAMBAMC_CINS = 1,
	BAMBAMC_CDEL = 2,
	BAMBAMC_CREF_SKIP = 3,
	BAMBAMC_CSOFT_CLIP = 4,
	BAMBAMC_CHARD_CLIP = 5,
	BAMBAMC_CPAD = 6,
	BAMBAMC_CEQUAL = 7,
	BAMBAMC_CDIFF = 8
};
#endif

