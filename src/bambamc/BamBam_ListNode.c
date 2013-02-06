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

#include <bambamc/BamBam_ListNode.h>

void BamBam_PrintStringFunction(FILE * file, void const * vstr)
{
	char const * str = (char const *)vstr;
	fprintf(file,"%s",str);
}

void BamBam_ListNode_Print(FILE * file, BamBam_ListNode const * node)
{
	if ( node )
	{
		if ( node->bamBamListPrintFunction )
		{
			node->bamBamListPrintFunction(file,node->entry);
		}
		else
		{
			fprintf(file,"<nullprintfunction>");
		}
	}
	else
	{
		fprintf(file,"<nullnode>");
	}
}

void BamBam_ListNode_PrintRec(FILE * file, BamBam_ListNode const * node)
{
	if ( node )
	{
		fprintf(file,"Node(");
		BamBam_ListNode_Print(file,node);
		fprintf(file,");");
		
		BamBam_ListNode_PrintRec(file,node->next);
	}
}

BamBam_ListNode * BamBam_ListNode_New()
{
	BamBam_ListNode * node = 0;
	
	node = (BamBam_ListNode *)malloc(sizeof(BamBam_ListNode));
	
	if ( ! node )
		return 0;
		
	node->next = 0;
	node->entry = 0;
	node->bamBamListFreeFunction = 0;
	node->bamBamListPrintFunction = 0;

	return node;
}

void BamBam_ListNode_Delete(BamBam_ListNode * node)
{
	if ( node )
	{
		if ( node->bamBamListFreeFunction )
			node->bamBamListFreeFunction(node->entry);
		free(node);
	}
}

void BamBam_ListNode_DeleteRec(BamBam_ListNode * node)
{
	if ( node )
	{
		if ( node->next )
		{
			BamBam_ListNode_DeleteRec(node->next);
			node->next = 0;
		}
		BamBam_ListNode_Delete(node);
	}
}
