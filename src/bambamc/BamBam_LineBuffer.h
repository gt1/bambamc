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
#if ! defined(BAMBAMC_LINEBUFFER_H)
#define BAMBAMC_LINEBUFFER_H

#include <bambamc/BamBam_Unused.h>
#include <stdio.h>

typedef struct _BamBam_LineBuffer
{
	FILE * file;
	
	char * buffer;
	unsigned int bufsize;
	int eof;
	char * tmpbuf;
	
	char * bufferptra;
	char * bufferptrin;
	char * bufferptrout;
	char * bufferptre;
} BamBam_LineBuffer;

extern BamBam_LineBuffer * BamBam_LineBuffer_New(FILE * rfile, int initsize) BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_LineBuffer_GetLine(BamBam_LineBuffer * object, char const **a, char const **e) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_LineBuffer_PutBack(BamBam_LineBuffer * object, char const *a);
extern BamBam_LineBuffer * BamBam_LineBuffer_Delete(BamBam_LineBuffer * object);
#endif
