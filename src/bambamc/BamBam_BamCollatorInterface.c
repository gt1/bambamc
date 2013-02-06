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

#include <bambamc/BamBam_BamCollatorInterface.h>

#include <bambamc/BamBam_List.h>
BamBam_List collatorList;

#include <bambamc/BamBam_BamCollator.h>

int BamBam_AllocBamCollator(char const * filename, char const * filetype, char const * tempdirname, int const keepOrphans)
{
	BamBam_BamCollator * collator = BamBam_BamCollator_New(tempdirname,16,16,filetype,filename);
	unsigned int id = -1;

	if ( ! 	collator )
		return id;
		
	collator->keepOrphans = keepOrphans;
	
	BamBam_ListNode * node = BamBam_ListNode_New();
	
	if ( ! node )
	{
		BamBam_BamCollator_Delete(collator);
		return id;
	}
	
	node->entry = collator;
	
	id = BamBam_List_Size(&collatorList);
	
	BamBam_ListNode_PushBack(&collatorList,node);
	
	return id;
}

#include <assert.h>

BamBam_BamCollator * BamBam_GetBamCollator(int id)
{
	BamBam_ListNode * node;

	if ( id < 0 )
		return 0;
	if ( id >= (int)(BamBam_List_Size(&collatorList)) ) 
		return 0;

	node = collatorList.first;
	
	while ( id )
	{
		assert ( node );
		node = node->next;
		--id;
	}
	
	if ( ! node )
		return 0;
	
	return node->entry;
}

void BamBam_FreeBamCollator(int const id)
{
	if ( id >= 0 && id < (int)(BamBam_List_Size(&collatorList)) )
	{
		int tid = id;
		BamBam_ListNode * node;
		node = collatorList.first;
	
		while ( tid )
		{
			assert ( node );
			node = node->next;
			--tid;
		}
	
		if ( node )
		{
			BamBam_BamCollator_Delete((BamBam_BamCollator *)(node->entry));
			node->entry = 0;
		}
	}
}

static void BamBam_ParseFastQ(char const * text, BamBam_FastQRead * reada, char const term)
{

	/* at line */
	assert ( *text == '@' );
	text++;
	reada->name = text;
	reada->namelength = 0;
		
	while ( *text != term )
		reada->namelength++, text++;

	assert ( *text == term );
	text++;
	
	/* sequence line */
	reada->seqlength = 0;
	reada->seq = text;
	while ( *text != term )
		reada->seqlength++, text++;
		
	assert ( *text == term );
	text++;
	
	/* plus line */
	while ( *text != term )
		text++;
	
	assert ( *text == term );
	text++;
	
	/* quality line */
	reada->qual = text;
}

#include <bambamc/BamBam_FormatAlignment.h>

int BamBam_ReadPair(int const id, BamBam_FastQRead * const reada, BamBam_FastQRead * const readb, void ** aligna, void ** alignb, char const term)
{
	BamBam_BamCollator * collator = BamBam_GetBamCollator(id);
	int getret = 0;
	int ret = BAMBAM_ALIGNMENT_TYPE_UNDEFINED;
	
	if ( !collator )
		return ret;
	
	while ( ret == BAMBAM_ALIGNMENT_TYPE_UNDEFINED )
	{
		if ( aligna )
			*aligna = 0;
		if ( alignb )
			*alignb = 0;
		BamBam_BamCollationHashEntry_Delete(collator->entryA); collator->entryA = 0;
		BamBam_BamCollationHashEntry_Delete(collator->entryB); collator->entryB = 0;

		getret = BamBam_BamCollator_Get(collator, &(collator->entryA), &(collator->entryB));
		
		/* a pair */
		if ( getret == 2 )
		{
			if ( aligna )
				*aligna = collator->entryA->entry;
			if ( alignb )
				*alignb = collator->entryB->entry;
			ret = BAMBAM_ALIGNMENT_TYPE_COMPLETE_PAIR;
		}
		/* single end */
		else if ( 
			getret == 1 && collator->entryA && 
			(!((BamBam_BamCollationHashEntry_GetFlags(collator->entryA)) & BAM_FPAIRED))
		)
		{
			if ( aligna )
				*aligna = collator->entryA->entry;
			ret = BAMBAM_ALIGNMENT_TYPE_SINGLE;
		}
		/* orphan, kept */
		else if ( getret == 1 && collator->keepOrphans )
		{
			if ( collator->entryA )
			{
				if ( aligna )
					*aligna = collator->entryA->entry;	
				ret = BAMBAM_ALIGNMENT_TYPE_ORPHAN1_PAIR;
			}
			else
			{
				*alignb = collator->entryB->entry;
				ret = BAMBAM_ALIGNMENT_TYPE_ORPHAN2_PAIR;
			}
		}
		/* orphan, not kept */
		else if ( getret == 1 && ! collator->keepOrphans )
		{
			BamBam_BamCollationHashEntry_Delete(collator->entryA); collator->entryA = 0;
			BamBam_BamCollationHashEntry_Delete(collator->entryB); collator->entryB = 0;
		}
		/* nothing */
		else
		{
			assert ( getret == 0 );
			ret = BAMBAM_ALIGNMENT_TYPE_NONE;
		}

		if ( aligna && *aligna )
		{
			int const lena = BamBam_PutAlignmentBuffer(*aligna,&(collator->bufferA),&(collator->bufferALength),term);
			
			if ( lena < 0 )
				ret = BAMBAM_ALIGNMENT_TYPE_NONE;
			else
				BamBam_ParseFastQ(collator->bufferA,reada,term);
		}
		if ( alignb && *alignb )
		{
			int const lenb = BamBam_PutAlignmentBuffer(*alignb,&(collator->bufferB),&(collator->bufferBLength),term);
			if ( lenb < 0 )
				ret = BAMBAM_ALIGNMENT_TYPE_NONE;
			else
				BamBam_ParseFastQ(collator->bufferB,readb,term);
		}
	}

	return ret;
}
