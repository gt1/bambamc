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

#include <bambamc/BamBam_BamHeader.h>
#include <bambamc/BamBam_BamHeaderInfo.h>

BamBam_BamHeader * BamBam_BamHeader_New(BamBam_BamHeaderInfo * info)
{
	BamBam_BamHeader * header = (BamBam_BamHeader *)malloc(sizeof(BamBam_BamHeader));
	
	if ( ! header )
		return 0;

	header->header = bam_header_init();
	
	if ( ! header->header )
	{
		free(header);
		return 0;
	}
	
	header->header->text = (char *)(info->cb->buffer);
	header->header->l_text = info->cb->bufferfill-1;
	
	info->cb->buffer = 0;
	info->cb->bufferfill = 0;
	info->cb->buffersize = 0;

	sam_header_parse(header->header);

	return header;
}

void BamBam_BamHeader_Delete(BamBam_BamHeader * header)
{
	if ( header )
	{
		if ( header->header )
			bam_header_destroy(header->header);
		header->header = 0;
		free(header);
	}
}
