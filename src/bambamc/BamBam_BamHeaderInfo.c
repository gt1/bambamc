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

#include <bambamc/BamBam_BamHeaderInfo.h>
#include <string.h>

int BamBam_BamHeaderInfo_ProduceHeaderText(BamBam_BamHeaderInfo * info)
{
	int ret = 0;
	BamBam_ListNode * node = 0;
	BamBam_CharBuffer * buffer = info->cb;
	
	BamBam_CharBuffer_Reset(buffer);
	
	BamBam_CharBuffer_PushString(buffer,"@HD\tVN:",ret); if ( ret < 0 ) return -1;
	BamBam_CharBuffer_PushString(buffer,info->version,ret); if ( ret < 0 ) return -1;
	BamBam_CharBuffer_PushString(buffer,"\tSO:",ret); if ( ret < 0 ) return -1;
	BamBam_CharBuffer_PushString(buffer,info->sortorder,ret); if ( ret < 0 ) return -1;
	BamBam_CharBuffer_PushString(buffer,"\n",ret); if ( ret < 0 ) return -1;
	
	node = info->chrlist->first;
	while ( ret >= 0 && node )
	{
		BamBam_Chromosome const * chr = (BamBam_Chromosome const *)(node->entry);
		
		BamBam_CharBuffer_PushString(buffer,"@SQ\tSN:",ret); if ( ret < 0 ) return -1;
		BamBam_CharBuffer_PushString(buffer,chr->name,ret); if ( ret < 0 ) return -1;
		BamBam_CharBuffer_PushString(buffer,"\tLN:",ret); if ( ret < 0 ) return -1;
		BamBam_CharBuffer_PushNumber(buffer,chr->length,ret); if ( ret < 0 ) return -1;
		BamBam_CharBuffer_PushString(buffer,"\n",ret); if ( ret < 0 ) return -1;
		
		node = node->next;
	}
	
	if ( info->plaintext )
		BamBam_CharBuffer_PushString(buffer,info->plaintext,ret); if ( ret < 0 ) return -1;	

	/* terminating NUL character */	
	if ( ret >= 0 )
		ret = BamBam_CharBuffer_PushChar(buffer,0);
	
	return ret;
}

BamBam_BamHeaderInfo * BamBam_BamHeaderInfo_Delete(BamBam_BamHeaderInfo * info)
{
	if ( info )
	{
		free(info->plaintext);
		free(info->version);
		free(info->sortorder);
		BamBam_List_Delete(info->chrlist);
		BamBam_CharBuffer_Delete(info->cb);
		free(info);
	}
	
	return 0;
}

BamBam_BamHeaderInfo * BamBam_BamHeaderInfo_New(char const * version, char const * sortorder, char const * plaintext)
{
	BamBam_BamHeaderInfo * info = 0;
	
	info = (BamBam_BamHeaderInfo *)malloc(sizeof(BamBam_BamHeaderInfo));
	
	if ( ! info )
		return BamBam_BamHeaderInfo_Delete(info);
		
	memset(info,0,sizeof(BamBam_BamHeaderInfo));

	info->sortorder = strdup(sortorder);

	if ( ! info->sortorder )
		return BamBam_BamHeaderInfo_Delete(info);
		
	info->version = strdup(version);

	if ( ! info->version )
		return BamBam_BamHeaderInfo_Delete(info);
	
	if ( plaintext )
	{
		info->plaintext = strdup(plaintext);

		if ( ! info->plaintext )
			return BamBam_BamHeaderInfo_Delete(info);
	}
	else
	{
		info->plaintext = 0;
	}
		
	info->cb = BamBam_CharBuffer_New();
	
	if ( ! info->cb )
		return BamBam_BamHeaderInfo_Delete(info);
		
	info->chrlist = BamBam_List_New();
	
	if ( ! info->chrlist )
		return BamBam_BamHeaderInfo_Delete(info);

	return info;
}

static void BamBam_Chromosome_NodeDelete(void * node)
{
	if ( node )
	{
		BamBam_Chromosome_Delete((BamBam_Chromosome *)node);
	}
}

int BamBam_BamHeaderInfo_AddChromosome(BamBam_BamHeaderInfo * info, char const * name, uint64_t len)
{
	BamBam_Chromosome * chr = BamBam_Chromosome_New(name,len);
	
	if ( ! chr )
		return -1;
		
	BamBam_ListNode * node = BamBam_ListNode_New();
	
	if ( ! node )
	{
		BamBam_Chromosome_Delete(chr);
		return -1;
	}
	
	node->entry = chr;
	node->bamBamListFreeFunction = BamBam_Chromosome_NodeDelete;
	node->bamBamListPrintFunction = 0;
	
	BamBam_ListNode_PushBack(info->chrlist,node);

	return 0;
}
