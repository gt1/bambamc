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
#if ! defined(BAMBAM_FORMATNUMBER_H)
#define BAMBAM_FORMATNUMBER_H

#include <bambamc/BamBam_Unused.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

extern char * BamBam_ConstructNumberFormatString(int numlen) BAMBAM_WARN_IF_UNUSEDRESULT;
extern char * BamBam_FormatNumberMinLen(uint64_t const num, int minlen) BAMBAM_WARN_IF_UNUSEDRESULT;
extern char * BamBam_FormatNumber(uint64_t const num) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
