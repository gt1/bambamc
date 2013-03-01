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
#if ! defined(BAMBAMC_SAMFILEDECODER_H)
#define BAMBAMC_SAMFILEDECODER_H

#include <bambamc/BamBam_LineBuffer.h>
#include <bambamc/BamBam_SamInfo.h>
#include <bambamc/BamBam_BamAlignmentPut.h>
#include <stdio.h>

#define BAMBAMC_SAMFILEDECODER_MANDATORYCOLUMNS 11

typedef struct _BamBam_SamFileDecoder
{
	FILE * closefile;
	BamBam_BamFileHeader * header;
	BamBam_LineBuffer * lb;
	BamBam_SamInfo * saminfo;
	BamBam_SamInfo_cptrpair fields[BAMBAMC_SAMFILEDECODER_MANDATORYCOLUMNS];
	BamBam_AlignmentPut * aput;
} BamBam_SamFileDecoder;

extern BamBam_BamSingleAlignment * BamBam_SamFileDecoder_LoadAlignment(BamBam_SamFileDecoder * object);
extern BamBam_SamFileDecoder * BamBam_SamFileDecoder_Delete(BamBam_SamFileDecoder * object);
extern BamBam_SamFileDecoder * BamBam_SamFileDecoder_NewFd(FILE * file);
extern BamBam_SamFileDecoder * BamBam_SamFileDecoder_NewFilename(char const * filename);
extern BamBam_SamFileDecoder * BamBam_SamFileDecoder_New(char const * filename);
#endif
