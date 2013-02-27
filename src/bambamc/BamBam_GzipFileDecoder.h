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
#if ! defined(BAMBAM_GZIPFILEDECODER_H)
#define BAMBAM_GZIPFILEDECODER_H

#include <bambamc/BamBam_BamSingleAlignment.h>
#include <bambamc/BamBam_BamFlagBase.h>

typedef struct _BamBam_GzipFileDecoder
{
	BamBam_GzipReader * reader;
	BamBam_BamSingleAlignment * alignment;
} BamBam_GzipFileDecoder;

extern BamBam_BamSingleAlignment * BamBam_GzipFileDecoder_DecodeAlignment(BamBam_GzipFileDecoder * object);
extern BamBam_GzipFileDecoder * BamBam_GzipFileDecoder_Delete(BamBam_GzipFileDecoder * object);
extern BamBam_GzipFileDecoder * BamBam_GzipFileDecoder_New(char const * filename);
#endif
