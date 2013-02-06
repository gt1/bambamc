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

#if ! defined(BAMBAM_CHARBUFFER_H)
#define BAMBAM_CHARBUFFER_H

#include <stdint.h>

typedef struct _BamBam_CharBuffer
{
	uint8_t * buffer;
	uint64_t buffersize;
	uint64_t bufferfill;
} BamBam_CharBuffer;

#include <bambamc/BamBam_Unused.h>

extern BamBam_CharBuffer * BamBam_CharBuffer_New() BAMBAM_WARN_IF_UNUSEDRESULT;
extern int BamBam_CharBuffer_PushChar(BamBam_CharBuffer * buffer, uint8_t c) BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_CharBuffer_Reset(BamBam_CharBuffer * buffer);
extern void BamBam_CharBuffer_Delete(BamBam_CharBuffer * buffer);
extern int BamBam_MapBase(uint8_t const c);
extern BamBam_CharBuffer * BamBam_CharBuffer_Clone(BamBam_CharBuffer const * o) BAMBAM_WARN_IF_UNUSEDRESULT;

#define BamBam_CharBuffer_PushCharQuick(buffer,c,ret) \
	do { \
		if (buffer->bufferfill < buffer->buffersize) \
			{ \
				buffer->buffer[buffer->bufferfill++] = c; \
			} \
		else \
			ret = BamBam_CharBuffer_PushChar(buffer,c); \
	} while(0);
	
#define BamBam_CharBuffer_PushLE(buffer,n,ret) \
	do { \
		unsigned int i; \
		for ( i = 0; i < 8*sizeof(n); i += 8 ) \
		{ \
			BamBam_CharBuffer_PushCharQuick(buffer,(uint8_t)(((uint8_t)(n >> i))&0xFFu),ret); \
			if ( ret < 0 ) \
			{ \
				ret = -1; \
				break; \
			} \
		} \
	} while(0);
	
#define BamBam_CharBuffer_PushString(buffer,s,ret) \
	do { \
		char const * c = s; \
		while ( *c ) \
		{ \
			BamBam_CharBuffer_PushCharQuick(buffer,*(c++),ret); \
			if ( ret < 0 ) \
			{ \
				break; \
			} \
		} \
	} while(0);

#define BamBam_CharBuffer_PushNumber(buffer,n,ret) \
	do { \
		uint64_t tn = n; \
		unsigned int numlen = 0; \
		unsigned int i = 0; \
		char * c = 0; \
		if ( !tn ) numlen = 1; \
		while ( tn ) \
			tn /= 10, numlen++; \
		c = (char *)alloca(numlen); \
		tn = n; \
		while ( i < numlen ) \
		{ \
			c[numlen-i-1] = (tn % 10)+'0'; \
			i++; \
			tn /= 10; \
		} \
		for ( i = 0; ret >= 0 && i < numlen; ++i ) \
			ret = BamBam_CharBuffer_PushChar(buffer,c[i]); \
	} while(0);
#endif

