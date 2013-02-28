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

#if ! defined(BAMBAM_BAMWRITER_H)
#define BAMBAM_BAMWRITER_H

#if ! defined(BAMBAMC_BAMONLY)
#include <bambamc/BamBam_BamHeader.h>
#endif

#include <bambamc/BamBam_BamAlignmentPut.h>
#include <bambamc/BamBam_BamHeaderInfo.h>

typedef struct _BamBam_BamWriter
{
	#if defined(BAMBAMC_BAMONLY)
	
	#else
	BamBam_BamHeader * header;
	bamFile outfile;	
	#endif
	BamBam_AlignmentPut * aput;
} BamBam_BamWriter;

extern BamBam_BamWriter * BamBam_BamWriter_Delete(BamBam_BamWriter * writer);
extern BamBam_BamWriter * BamBam_BamWriter_New(
	BamBam_BamHeaderInfo * info,
	char const * filename,
	int compressionLevel) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
