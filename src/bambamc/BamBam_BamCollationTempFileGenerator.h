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

#if ! defined(BAMBAM_BAMCOLLATIONTEMPFILEGENERATOR_H)
#define BAMBAM_BAMCOLLATIONTEMPFILEGENERATOR_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_FormatNumber.h>
#include <bambamc/BamBam_List.h>

typedef struct _BamBam_BamCollationTempFileGenerator
{
	char * prefix;
	uint64_t nextid;
	BamBam_List * tempfilenames;
} BamBam_BamCollationTempFileGenerator;

extern void BamBam_BamCollationTempFileGenerator_Print(FILE * file, BamBam_BamCollationTempFileGenerator const * gen);
extern char * BamBam_ComputeTempFileName(char const * prefix, uint64_t const id) BAMBAM_WARN_IF_UNUSEDRESULT;
extern char * BamBam_BamCollationTempFileGenerator_GetNextTempFileName(BamBam_BamCollationTempFileGenerator * gen) BAMBAM_WARN_IF_UNUSEDRESULT;
extern BamBam_BamCollationTempFileGenerator * BamBam_BamCollationTempFileGenerator_New(char const * prefix) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_BamCollationTempFileGenerator_Delete(BamBam_BamCollationTempFileGenerator * gen);
#endif
