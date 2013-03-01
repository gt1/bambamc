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

#include <bambamc/BamBam_LineBuffer.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

BamBam_LineBuffer * BamBam_LineBuffer_New(FILE * rfile, int initsize)
{
	BamBam_LineBuffer * object = 0;
	size_t iobytes = 0;
	
	object = (BamBam_LineBuffer *)malloc(sizeof(BamBam_LineBuffer));
	
	if ( ! object )
		return BamBam_LineBuffer_Delete(object);
		
	memset(object,0,sizeof(BamBam_LineBuffer));
	
	object->file = rfile;

	if ( ! object->file )
		return BamBam_LineBuffer_Delete(object);
		
	object->buffer = (char *)malloc(initsize);
	
	if ( ! object->buffer )
		return BamBam_LineBuffer_Delete(object);
		
	object->bufsize = initsize;
	
	object->bufferptra = object->buffer;
	object->bufferptre = object->buffer + object->bufsize;
	object->bufferptrout = object->buffer;
	
	iobytes = fread(object->bufferptra,1,object->bufsize,object->file);
	
	object->bufferptrin = object->bufferptra + iobytes;
	
	if ( ferror(object->file) )
		return BamBam_LineBuffer_Delete(object);
	if ( feof(object->file) )
		object->eof = 1;
	
	return object;
}

BamBam_LineBuffer * BamBam_LineBuffer_Delete(BamBam_LineBuffer * object)
{
	if ( object )
	{
		if ( object->buffer )
		{
			free(object->buffer);
			object->buffer = 0;
		}
		if ( object->tmpbuf )
		{
			free(object->tmpbuf);
			object->tmpbuf = 0;
		}
		free(object);
	}
	return 0;
}

int BamBam_LineBuffer_GetLine(BamBam_LineBuffer * object, char const **a, char const **e)
{
	while ( 1 )
	{
		/* end of line pointer */
		char * lineend = object->bufferptrout;

		/* search for end of buffer or line end */
		while ( lineend != object->bufferptrin && *(lineend) != '\n' )
			++lineend;
		
		/* we reached the end of the data currently in memory */
		if ( lineend == object->bufferptrin )
		{
			/* reached end of file, return what we have */
			if ( object->eof )
			{
				/* this is the last line we will return */
				if ( object->bufferptrout != object->bufferptrin )
				{
					/* if file ends with a newline */
					if ( object->bufferptrin[-1] == '\n' )
					{
						*a = object->bufferptrout;
						*e = object->bufferptrin-1;
						object->bufferptrout = object->bufferptrin;
						return 0;
					}
					/* otherwise we append an artifical newline */
					else
					{
						uint64_t const numbytes = lineend - object->bufferptrout;
						char * tmpbuf = (char *)malloc(numbytes+1);
						
						if ( ! tmpbuf )
							return -1;
							
						memcpy(tmpbuf,object->bufferptrout,numbytes);
						tmpbuf[numbytes] = '\n';
						
						free(object->buffer);
						
						object->buffer = tmpbuf;
						object->bufsize = numbytes+1;
						object->bufferptra = tmpbuf;
						object->bufferptre = tmpbuf + object->bufsize;
						object->bufferptrin = object->bufferptre;
						object->bufferptrout = object->bufferptre;
						
						*a = object->bufferptra;
						*e = object->bufferptre - 1;
						return 0;	
					}
				}
				else
				{
					return -1;
				}
			}
			/* we need to read more data */
			else
			{
				/* do we need to extend the buffer? */
				if ( 
					object->bufferptrout == object->bufferptra
					&&
					object->bufferptrin == object->bufferptre 
				)
				{
					unsigned int const newbufsize = object->bufsize ? 2*object->bufsize : 1;
					char * newbuf = realloc(object->buffer,newbufsize);

					if ( newbuf )
					{
						/* fprintf(stderr,"extended buffer to %d bytes\n", newbufsize); */
						object->bufferptre   = newbuf + newbufsize;
						object->bufferptrout = newbuf + (object->bufferptrout - object->bufferptra);
						object->bufferptrin  = newbuf + (object->bufferptrin - object->bufferptra);
						object->bufferptra   = newbuf;
						object->buffer       = newbuf;
						object->bufsize      = newbufsize;
					}
					else
					{
						return -1;
					}
				}
				else
				{
					/* move data to front and fill rest of buffer */
					uint64_t const used   = object->bufferptrin  - object->bufferptrout;
					uint64_t const unused = object->bufsize - used;
					size_t iobytes = 0;
					
					memmove(object->bufferptra, object->bufferptrout,used);
					
					object->bufferptrout = object->bufferptra;
					object->bufferptrin  = object->bufferptrout + used;
					
					iobytes = fread(object->bufferptrin,1,unused,object->file);
					
					if ( ferror(object->file) )
						return -1;
					if ( feof(object->file) )
						object->eof = 1;
						
					/* fprintf(stderr,"Got %d bytes\n", iobytes); */
					
					object->bufferptrin += iobytes;
				}
			}
		}
		else
		{
			*a = object->bufferptrout;
			*e = lineend;
			assert ( *lineend == '\n' );
			object->bufferptrout = lineend+1;
			return 0;
		}		
	}

	return -1;
}
void BamBam_LineBuffer_PutBack(BamBam_LineBuffer * object, char const *a)
{
	object->bufferptrout = (char *)a;
}
