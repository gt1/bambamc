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
#include <bambamc/BamBam_SamBamFileDecoder.h>

BamBam_SamBamFileDecoder * BamBam_SamBamFileDecoder_Delete(BamBam_SamBamFileDecoder * object)
{
	if ( object )
	{
		if ( object->bamdecoder )
			BamBam_BamFileDecoder_Delete(object->bamdecoder);
		if ( object->samdecoder )
			BamBam_SamFileDecoder_Delete(object->samdecoder);
	}

	free(object);

	return 0;
}

BamBam_BamSingleAlignment * BamBam_SamBamFileDecoder_DecodeAlignment(BamBam_SamBamFileDecoder * object)
{
	if ( object && object->bamdecoder )
		return BamBam_BamFileDecoder_DecodeAlignment(object->bamdecoder);
	else if ( object && object->samdecoder )
		return BamBam_SamFileDecoder_LoadAlignment(object->samdecoder);
	else
		return 0;
}

static int BamBam_SamBamFileDecoder_ContainsLetter(char const * s, char const c)
{
	while ( *s )
		if ( *s == c )
			return 1;
		else
			++s;
			
	return 0;
}

BamBam_SamBamFileDecoder * BamBam_SamBamFileDecoder_New(char const * filename, char const * mode)
{
	if ( ! BamBam_SamBamFileDecoder_ContainsLetter(mode,'r') )
		return 0;
		
	BamBam_SamBamFileDecoder * object = (BamBam_SamBamFileDecoder *)malloc(sizeof(BamBam_SamBamFileDecoder));
	
	if ( ! object )
		return BamBam_SamBamFileDecoder_Delete(object);
		
	memset(object,0,sizeof(BamBam_SamBamFileDecoder));
	
	if ( BamBam_SamBamFileDecoder_ContainsLetter(mode,'b') )
	{
		object->bamdecoder = BamBam_BamFileDecoder_New(filename);
	
		if ( ! object->bamdecoder || ! object->bamdecoder->header )
			return BamBam_SamBamFileDecoder_Delete(object);	
			
		object->header = object->bamdecoder->header;
	}
	else
	{
		object->samdecoder = BamBam_SamFileDecoder_New(filename);
	
		if ( ! object->samdecoder || ! object->samdecoder->header )
			return BamBam_SamBamFileDecoder_Delete(object);	
			
		object->header = object->samdecoder->header;
	}
	
	return object;
}
