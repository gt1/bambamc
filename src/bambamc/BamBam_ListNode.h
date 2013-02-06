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

#if ! defined(BAMBAM_LISTNODE_H)
#define BAMBAM_LISTNODE_H

#include <bambamc/BamBam_Unused.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*bamBamListFreeFunction)(void *);
typedef void (*bamBamListPrintFunction)(FILE * file, void const *);

typedef struct _BamBam_ListNode
{
	struct _BamBam_ListNode * next;
	void * entry;
	bamBamListFreeFunction bamBamListFreeFunction;
	bamBamListPrintFunction bamBamListPrintFunction;
} BamBam_ListNode;

extern void BamBam_PrintStringFunction(FILE * file, void const * vstr);
extern void BamBam_ListNode_Print(FILE * file, BamBam_ListNode const * node);
extern void BamBam_ListNode_PrintRec(FILE * file, BamBam_ListNode const * node);
extern BamBam_ListNode * BamBam_ListNode_New() BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_ListNode_Delete(BamBam_ListNode * node);
extern void BamBam_ListNode_DeleteRec(BamBam_ListNode * node);
#endif
