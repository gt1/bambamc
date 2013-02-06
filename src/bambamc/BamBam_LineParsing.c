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

#include <bambamc/BamBam_LineParsing.h>
#include <bambamc/BamBam_CharBuffer.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

char const * BamBam_findLineEnd(char const * s)
{
	while ( *s && (*s != '\n') )
		++s;
	return s;
}
char const * BamBam_skipEndOfLine(char const * s)
{
	if ( ! *s )
		return s;

	while ( *s && isspace(*s) )
		s++;
	
	return s;
}
char const * BamBam_nextLine(char const * s)
{
	return BamBam_skipEndOfLine(BamBam_findLineEnd(s));
}
unsigned int BamBam_getLineLength(char const * s)
{
	return BamBam_findLineEnd(s)-s;
}
int BamBam_copyLine(char const * s, BamBam_CharBuffer * buffer)
{
	unsigned int const l = BamBam_getLineLength(s);
	unsigned int i = 0;
	int ret = 0;
	
	for ( i = 0; (! ret) && i < l; ++i )
		ret = BamBam_CharBuffer_PushChar(buffer,s[i]);
	
	if ( ! ret )
		ret = BamBam_CharBuffer_PushChar(buffer,'\n');
		
	return ret;
}
int BamBam_isValidLine(char const * s, char const ** filtered)
{
	char const * sa;
	char const * se;
	
	if ( ! *s )
		return 0;
	if ( s[0] != '@' )
		return 0;
	
	sa = s+1;
	se = sa;
	while ( *se && *se != '\n' && *se != '\t' )
		++se;
	
	while ( *filtered )
	{
		char const * filter = *filtered;
		
		if ( (strlen(filter) == (size_t)(se-sa)) && memcmp(filter,sa,se-sa) == 0 )
			return 0;
			
		++filtered;
	}
	
	return 1;
}
char * BamBam_filterHeader(char const * header, char const ** filters)
{
	BamBam_CharBuffer * buffer = BamBam_CharBuffer_New();
	char * filtered = 0;
	int ret = 0;
	
	if ( ! buffer )
		return 0;
		
	while ( *header )
	{
		if ( BamBam_isValidLine(header,filters) )
		{
			ret = BamBam_copyLine(header,buffer);
			
			if ( ret < 0 )
			{
				BamBam_CharBuffer_Delete(buffer);
				return 0;
			}
		}
		header = BamBam_nextLine(header);
	}
	
	/* append terminator */
	ret = BamBam_CharBuffer_PushChar(buffer,0);
	
	if ( ret < 0 )
	{
		BamBam_CharBuffer_Delete(buffer);
		return 0;
	}
	
	filtered = strdup((char const *)(buffer->buffer));
	
	if ( ! filtered )
	{
		BamBam_CharBuffer_Delete(buffer);
		return 0;
	}
		
	BamBam_CharBuffer_Delete(buffer);
	
	return filtered;
}
