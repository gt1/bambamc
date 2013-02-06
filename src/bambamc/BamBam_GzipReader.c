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
#include <bambamc/BamBam_GzipReader.h>
#include <stdlib.h>
#include <string.h>

BamBam_GzipReader * BamBam_GzipReader_New_Filename(char const * filename)
{
	BamBam_GzipReader * object = 0;
	
	object = (BamBam_GzipReader *)malloc(sizeof(BamBam_GzipReader));
	
	if ( ! object )
		return BamBam_GzipReader_Delete(object);
		
	memset(object,0,sizeof(BamBam_GzipReader));
	
	object->file = gzopen(filename,"rb");
	
	if ( ! object->file )
		return BamBam_GzipReader_Delete(object);	

	return object;
}

BamBam_GzipReader * BamBam_GzipReader_New_Fd(int fd)
{
	BamBam_GzipReader * object = 0;
	
	object = (BamBam_GzipReader *)malloc(sizeof(BamBam_GzipReader));
	
	if ( ! object )
		return BamBam_GzipReader_Delete(object);
		
	memset(object,0,sizeof(BamBam_GzipReader));
	
	object->file = gzdopen(fd,"rb");
	
	if ( ! object->file )
		return BamBam_GzipReader_Delete(object);	

	return object;
}

BamBam_GzipReader * BamBam_GzipReader_New(char const * filename)
{
	if ( !filename || !strcmp(filename,"-") )
		return BamBam_GzipReader_New_Fd(STDIN_FILENO);
	else
		return BamBam_GzipReader_New_Filename(filename);
}

BamBam_GzipReader * BamBam_GzipReader_Delete(BamBam_GzipReader * object)
{
	if ( object )
	{
		if ( object->file )
		{
			gzclose(object->file);
			object->file = 0;
		}
		free(object);	
	}

	return 0;
}

int BamBam_GzipReader_Peek(BamBam_GzipReader * object)
{
	int c = -1;

	if ( ! object || ! object->file )
		return -1;

	c = gzgetc(object->file);
	
	if ( c < 0 )
		return -1;
		
	if ( gzungetc(c,object->file) < 0 )
		return -1;
	
	return c;
}

int BamBam_GzipReader_Getc(BamBam_GzipReader * object)
{
	return gzgetc(object->file);
}

int BamBam_GzipReader_GetInt32(BamBam_GzipReader * object, int32_t * v)
{
	int i = 0;
	*v = 0;
	
	for ( i = 0; i < 4; ++i )
	{
		int const c = BamBam_GzipReader_Getc(object);
		if ( c < 0 )
			return -1;
		*v |= ((int32_t)c) << (i*8);
	}
	
	return 0;
}
int BamBam_GzipReader_GetUInt32(BamBam_GzipReader * object, uint32_t * v)
{
	int i = 0;
	*v = 0;
	
	for ( i = 0; i < 4; ++i )
	{
		int const c = BamBam_GzipReader_Getc(object);
		if ( c < 0 )
			return -1;
		*v |= ((uint32_t)c) << (i*8);
	}
	
	return 0;
}
int BamBam_GzipReader_Read(BamBam_GzipReader * object, char * c, int l)
{
	return gzread(object->file,c,l);
}
