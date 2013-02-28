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

#include <bambamc/BamBam_BamCollationHashEntry.h>
#include <bambamc/BamBam_BamFlagBase.h>

int BamBam_BamHashEntry_Compare(BamBam_BamCollationHashEntry const * A, BamBam_BamCollationHashEntry const * B)
{
	int cmpres = strcmp(A->qname,B->qname);
	int Amate1 = (BamBam_BamCollationHashEntry_GetFlags(A) & BAMBAMC_FREAD1) != 0;
	int Bmate1 = (BamBam_BamCollationHashEntry_GetFlags(B) & BAMBAMC_FREAD1) != 0;
	
	if ( cmpres != 0 )
		return cmpres;
	else if ( Amate1 < Bmate1 )
		return -1;
	else if ( Amate1 > Bmate1 )
		return 1;
	else
		return 0;

}

int BamBam_BamHashEntry_CompareVerbose(BamBam_BamCollationHashEntry const * A, BamBam_BamCollationHashEntry const * B)
{
	int cmpres = strcmp(A->qname,B->qname);
	int Amate1 = (BamBam_BamCollationHashEntry_GetFlags(A) & BAMBAMC_FREAD1) != 0;
	int Bmate1 = (BamBam_BamCollationHashEntry_GetFlags(B) & BAMBAMC_FREAD1) != 0;
	
	fprintf(stderr,"Comparing %s and %s result %d\n", A->qname, B->qname, cmpres);
	
	if ( cmpres != 0 )
		return cmpres;
	else if ( Amate1 < Bmate1 )
		return -1;
	else if ( Amate1 > Bmate1 )
		return 1;
	else
		return 0;

}

int BamBam_BamHashEntry_CompareVoidPtr(const void * VA, const void * VB)
{
	BamBam_BamCollationHashEntry const ** A = (BamBam_BamCollationHashEntry const **)VA;
	BamBam_BamCollationHashEntry const ** B = (BamBam_BamCollationHashEntry const **)VB;	
	return BamBam_BamHashEntry_Compare(*A,*B);
}

void BamBam_BamCollationHashEntry_Delete(BamBam_BamCollationHashEntry * hashentry)
{
	if ( hashentry )
	{
		if ( hashentry->entry )
			#if defined(BAMBAMC_BAMONLY)
			BamBam_BamSingleAlignment_Delete(hashentry->entry);
			#else
			bam_destroy1(hashentry->entry);
			#endif
		free(hashentry->qname);
		free(hashentry);
	}
}

uint32_t BamBam_BamCollationHashEntry_GetFlags(BamBam_BamCollationHashEntry const * hashentry)
{
	#if defined(BAMBAMC_BAMONLY)
	return BamBam_BamSingleAlignment_GetFlags(hashentry->entry);
	#else
	bam1_core_t const * core = &(hashentry->entry->core);
        /* flags */
        uint32_t const flags = core->flag;
                        
        return flags;
        #endif                   
}

#if defined(BAMBAMC_BAMONLY)
BamBam_BamCollationHashEntry * BamBam_BamCollationHashEntry_NewDup(BamBam_BamSingleAlignment * alignment)
{
	BamBam_BamSingleAlignment * entry = 0;
	BamBam_BamCollationHashEntry * hashentry = 0;
	char const * qname = 0;

	entry = BamBam_BamSingleAlignment_Clone(alignment);
	
	if ( ! entry )
		return 0;
		
	hashentry = (BamBam_BamCollationHashEntry *)malloc(sizeof(BamBam_BamCollationHashEntry));
	
	if ( ! hashentry )
	{
		BamBam_BamSingleAlignment_Delete(entry);
		return 0;
	}
	
	hashentry->entry = entry;
	hashentry->qnamelen = 0;
	hashentry->qname = 0;
	
	qname = BamBam_BamSingleAlignment_GetReadName(entry);
	
	if ( ! qname )
	{
		BamBam_BamSingleAlignment_Delete(entry);
		free(hashentry);
		return 0;
	}
	
	hashentry->qname = strdup(qname);
	
	if ( ! hashentry->qname )
	{
		BamBam_BamSingleAlignment_Delete(entry);
		free(hashentry);
		return 0;	
	}
	
	hashentry->qnamelen = strlen(hashentry->qname);	
	hashentry->hashvalue = hashDefaultSeed((uint8_t const *)hashentry->qname,hashentry->qnamelen);

	return hashentry;
}
#else
BamBam_BamCollationHashEntry * BamBam_BamCollationHashEntry_NewDup(bam1_t * alignment)
{
	bam1_t * entry = 0;	
	BamBam_BamCollationHashEntry * hashentry = 0;
	char const * qname = 0;

	entry = bam_dup1(alignment);
	
	if ( ! entry )
		return 0;
		
	hashentry = (BamBam_BamCollationHashEntry *)malloc(sizeof(BamBam_BamCollationHashEntry));
	
	if ( ! hashentry )
	{
		bam_destroy1(entry);
		return 0;
	}
	
	hashentry->entry = entry;
	hashentry->qnamelen = 0;
	hashentry->qname = 0;
	
	qname = bam1_qname(entry);
	
	if ( ! qname )
	{
		bam_destroy1(entry);
		free(hashentry);
		return 0;
	}
	
	hashentry->qname = strdup(qname);
	
	if ( ! hashentry->qname )
	{
		bam_destroy1(entry);
		free(hashentry);
		return 0;	
	}
	
	hashentry->qnamelen = strlen(hashentry->qname);	
	hashentry->hashvalue = hashDefaultSeed((uint8_t const *)hashentry->qname,hashentry->qnamelen);

	return hashentry;
}
#endif
