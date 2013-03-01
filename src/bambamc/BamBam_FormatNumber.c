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

#include <bambamc/BamBam_FormatNumber.h>
#include <bambamc/BamBam_StrDup.h>
#include <stdio.h>
#include <alloca.h>

char * BamBam_ConstructNumberFormatString(int numlen)
{
	int lennumlen = 0;
	int tnumlen = 0;
	unsigned int fslen = 0;
	char * fs = 0;
	
	tnumlen = numlen;
	while ( tnumlen )
	{
		tnumlen /= 10;
		lennumlen++;
	}
	
	fslen = 6 + lennumlen + 1;
	
	fs = (char *)malloc(fslen);
	
	if ( ! fs )
		return 0;
	
	snprintf(fs,fslen,"%%0%dllu",numlen);
	
	return fs;
}

char * BamBam_FormatNumberMinLenAlt(uint64_t const num, int const minlen)
{
	/* length of number in decimal repr */
	int numlen = 0;
	/* tmp */
	uint64_t tnum;
	/* string on stack */
	char * s = 0;
	char * t = 0;
	
	if ( !num )
		numlen = 1;
	else
	{
		/* count number of digits */
		tnum = num;
		while ( tnum )
		{
			tnum /= 10;
			numlen++;
		}
	}
	
	numlen = (numlen >= minlen) ? numlen : minlen;
	
	s = (char *)alloca(numlen+1);
	s[numlen] = 0;
	
	t = s + numlen;
	tnum = num;
	
	while ( t != s )
	{
		*(--t) = '0' + (tnum%10);
		tnum /= 10;
	}
	
	return BamBam_StrDup(s);
}

char * BamBam_FormatNumberMinLen(uint64_t const num, int minlen)
{
	uint64_t tnum;
	int numlen;
	char * smem = 0;
	char * fs = 0;
	
	tnum = num;
	numlen = 0;
	
	while ( tnum )
	{
		tnum /= 10;
		numlen++;
	}
	
	if ( ! numlen )
		numlen = 1;

	numlen = (numlen >= minlen) ? numlen : minlen;
		
	smem = (char *)malloc(numlen+1);
	
	if ( ! smem )
		return 0;
		
	fs = BamBam_ConstructNumberFormatString(numlen);
	
	if ( ! fs )
	{
		free(smem);
		return 0;
	}
	
	snprintf(smem,numlen+1,fs,(unsigned long long)num);
	
	free(fs);
	
	return smem;
}

char * BamBam_FormatNumber(uint64_t const num)
{
	return BamBam_FormatNumberMinLen(num,-1);
}
