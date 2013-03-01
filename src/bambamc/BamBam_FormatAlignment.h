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

#if ! defined(BAMBAM_FORMATALIGNMENT_H)
#define BAMBAM_FORMATALIGNMENT_H

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_BamSingleAlignment.h>
#include <bambamc/BamBam_Config.h>
#include <stdint.h>

extern int BamBam_PutAlignmentFastQBuffer(BamBam_BamSingleAlignment * alignment,  char ** buffer,  unsigned int * bufferlen,  char const term);

#if defined(BAMBAMC_BAMONLY)
#else
#if defined(HAVE_SAM_H)
#include <sam.h>
#endif
#if defined(HAVE_SAMTOOLS_SAM_H)
#include <samtools/sam.h>
#endif

extern int BamBam_PutAlignmentBuffer(bam1_t const * alignment, char ** buffer, unsigned int * bufferlen, char const term) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif

#endif
