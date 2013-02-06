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

#if ! defined(BAMBAM_BAMCOLLATORINTERFACE_H)
#define BAMBAM_BAMCOLLATORINTERFACE_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_FastQRead.h>

extern int BamBam_AllocBamCollator(char const * filename, char const * filetype, char const * tempdirname, int const keepOrphans) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_FreeBamCollator(int const id);
extern int BamBam_ReadPair(int const id, BamBam_FastQRead * const reada, BamBam_FastQRead * const readb, void ** aligna, void ** alignb, char const term) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
