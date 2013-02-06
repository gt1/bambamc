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
#include <bambamc/BamBam_List.h>

void BamBam_ListNode_PushBack(BamBam_List * list, BamBam_ListNode * listnode)
{
	if ( ! list->last )
	{
		list->first = list->last = listnode;
	}
	else
	{
		list->last->next = listnode;
		list->last = listnode;
	}
}

void BamBam_ListNode_PushFront(BamBam_List * list, BamBam_ListNode * listnode)
{
	if ( ! list->last )
	{
		list->first = list->last = listnode;
	}
	else
	{
		listnode->next = list->first;
		list->first = listnode;
	}
}

BamBam_List * BamBam_List_New()
{
	BamBam_List * list = 0;

	list = (BamBam_List *)malloc(sizeof(BamBam_List));
	
	if ( ! list )
		return 0;
		
	list->first = 0;
	list->last = 0;
		
	return list;
}

void BamBam_List_Delete(BamBam_List * list)
{
	if ( list )
	{
		if ( list->first )
			BamBam_ListNode_DeleteRec(list->first);
		free (list);
	}
}

void BamBam_List_Print(FILE * file, BamBam_List const * list)
{
	if ( list )
	{
		BamBam_ListNode_PrintRec(file,list->first);
	}
}

uint64_t BamBam_List_Size(BamBam_List * list)
{
	uint64_t length = 0;
	BamBam_ListNode * node = list->first;
	
	while ( node )
	{
		length++;
		node = node->next;
	}
	
	return length;
}