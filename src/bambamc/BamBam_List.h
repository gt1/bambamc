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

#if ! defined(BAMBAM_LIST_HPP)
#define BAMBAM_LIST_HPP

#include <bambamc/BamBam_Unused.h>
#include <bambamc/BamBam_ListNode.h>
#include <stdint.h>

typedef struct _BamBam_List
{
	BamBam_ListNode * first;
	BamBam_ListNode * last;
} BamBam_List;

extern void BamBam_ListNode_PushBack(BamBam_List * list, BamBam_ListNode * listnode);
extern void BamBam_ListNode_PushFront(BamBam_List * list, BamBam_ListNode * listnode);
extern BamBam_List * BamBam_List_New() BAMBAM_WARN_IF_UNUSEDRESULT;
extern void BamBam_List_Delete(BamBam_List * list);
extern void BamBam_List_Print(FILE * file, BamBam_List const * list);
extern uint64_t BamBam_List_Size(BamBam_List * list) BAMBAM_WARN_IF_UNUSEDRESULT;
#endif
