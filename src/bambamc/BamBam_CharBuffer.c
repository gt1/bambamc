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

#include <bambamc/BamBam_CharBuffer.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

BamBam_CharBuffer * BamBam_CharBuffer_Clone(BamBam_CharBuffer const * o)
{
	BamBam_CharBuffer * charbuffer = BamBam_CharBuffer_New();
	
	if ( ! charbuffer )
		return 0;

	charbuffer->buffer = (uint8_t *)malloc(o->buffersize);
	
	if ( ! charbuffer->buffer )
	{
		BamBam_CharBuffer_Delete(charbuffer);
		return 0;
	}
	
	memcpy(charbuffer->buffer,o->buffer,o->buffersize);
	charbuffer->buffersize = o->buffersize;
	charbuffer->bufferfill = o->bufferfill;

	return charbuffer;	                        
}

BamBam_CharBuffer * BamBam_CharBuffer_New()
{
	BamBam_CharBuffer * charbuffer = 0;
	
	charbuffer = (BamBam_CharBuffer *)malloc(sizeof(BamBam_CharBuffer));
	
	if ( ! charbuffer )
		return 0;
	
	memset(charbuffer,0,sizeof(BamBam_CharBuffer));
	
	return charbuffer;
}

void BamBam_CharBuffer_Reset(BamBam_CharBuffer * buffer)
{
	buffer->bufferfill = 0;
}

int BamBam_CharBuffer_PushChar(BamBam_CharBuffer * buffer, uint8_t c)
{
	if ( buffer->bufferfill == buffer->buffersize )
	{
		uint64_t newbuffersize = (buffer->buffersize) ? (2*buffer->buffersize) : 1;
		uint8_t * newbuffer = 0;
		
		newbuffer = (uint8_t *)malloc(newbuffersize*sizeof(uint8_t));
		
		if ( ! newbuffer )
			return -1;
		
		memcpy(newbuffer,buffer->buffer,buffer->bufferfill);
		
		free(buffer->buffer);
		buffer->buffer = newbuffer;
		buffer->buffersize = newbuffersize;
	}
	
	assert ( buffer->bufferfill < buffer->buffersize );
	
	buffer->buffer [ buffer->bufferfill ++ ] = c;
	
	return 0;
}

void BamBam_CharBuffer_Delete(BamBam_CharBuffer * buffer)
{
	if ( buffer )
	{
		free(buffer->buffer);
		free(buffer);
	}
}

int BamBam_MapBase(uint8_t const c)
{
	switch ( c )
	{
		case 'A': return 0;
		case 'C': return 1;
		case 'G': return 2;
		case 'T': return 3;
		default:  return 4;
	}
}

