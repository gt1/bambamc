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
#include <bambamc/BamBam_StrDup.h>
#include <string.h>
#include <stdlib.h>

char * BamBam_StrDup(char const * s)
{
	size_t const len = strlen(s);
	char * c = (char *)malloc(len+1);
	
	if ( ! c )
		return 0;
	
	memcpy(c,s,len);
	c[len] = 0;
	
	return c;
}

char * BamBam_StrCat(char const * sa, char const * sb)
{
	size_t const lena = strlen(sa);
	size_t const lenb = strlen(sb);
	char * c = (char *)malloc(lena+lenb+1);
	
	if ( ! c )
		return 0;
		
	memcpy(c+0   ,sa,lena);
	memcpy(c+lena,sb,lenb);
	c[lena+lenb] = 0;
	
	return c;
}
