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
#include <bambamc/BamBam_GzipWriter.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

BamBam_GzipWriter * BamBam_GzipWriter_New_Filename(char const * filename, int level)
{
	BamBam_GzipWriter * object = 0;
	char modestr[4] = { 'w', 'b', '0', 0 };
	
	if ( level < 0 || level > 9 )
		return 0;
		
	modestr[2] = level + '0';
	
	object = (BamBam_GzipWriter *)malloc(sizeof(BamBam_GzipWriter));
	
	if ( ! object )
		return BamBam_GzipWriter_Delete(object,0);
		
	memset(object,0,sizeof(BamBam_GzipWriter));
	
	object->file = gzopen(filename,&modestr[0]);
	
	if ( ! object->file )
		return BamBam_GzipWriter_Delete(object,0);	

	return object;
}

BamBam_GzipWriter * BamBam_GzipWriter_New_Fd(int fd, int level)
{
	BamBam_GzipWriter * object = 0;

	char modestr[4] = { 'w', 'b', '0', 0 };
	
	if ( level < 0 || level > 9 )
		return 0;
		
	modestr[2] = level + '0';
	
	object = (BamBam_GzipWriter *)malloc(sizeof(BamBam_GzipWriter));
	
	if ( ! object )
		return BamBam_GzipWriter_Delete(object,0);
		
	memset(object,0,sizeof(BamBam_GzipWriter));
	
	object->file = gzdopen(fd,&modestr[0]);
	
	if ( ! object->file )
		return BamBam_GzipWriter_Delete(object,0);	

	return object;
}

BamBam_GzipWriter * BamBam_GzipWriter_New(char const * filename, int level)
{
	if ( !filename || !strcmp(filename,"-") )
		return BamBam_GzipWriter_New_Fd(STDIN_FILENO,level);
	else
		return BamBam_GzipWriter_New_Filename(filename,level);
}

BamBam_GzipWriter * BamBam_GzipWriter_Delete(BamBam_GzipWriter * object, int * state)
{
	if ( state )
		*state = 0;

	if ( object )
	{
		if ( object->file )
		{
			int const r = gzclose(object->file);
			
			if ( state )
			{
				if ( r == Z_OK )
					*state = 0;
				else
					*state = -1;
			}
			
			object->file = 0;
		}
		free(object);	
	}

	return 0;
}

int BamBam_GzipWriter_Putc(BamBam_GzipWriter * object, int c)
{
	return gzputc(object->file,c);
}

int BamBam_GzipWriter_PutInt32(BamBam_GzipWriter * object, int32_t v)
{
	int i = 0;
	
	for ( i = 0; i < 4; ++i )
	{
		int const c = BamBam_GzipWriter_Putc(object, (v >> (8*i)) & 0xFF);
		if ( c < 0 )
			return -1;
	}
	
	return 0;
}
int BamBam_GzipWriter_PutUInt32(BamBam_GzipWriter * object, uint32_t v)
{
	int i = 0;
	
	for ( i = 0; i < 4; ++i )
	{
		int const c = BamBam_GzipWriter_Putc(object, (v >> (8*i)) & 0xFF);

		if ( c < 0 )
			return -1;
	}
	
	return 0;
}
int BamBam_GzipWriter_Write(BamBam_GzipWriter * object, char const * c, int l)
{
	return gzwrite(object->file,c,l);
}
