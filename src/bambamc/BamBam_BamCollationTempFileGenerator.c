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

#include <bambamc/BamBam_BamCollationTempFileGenerator.h>
#include <string.h>
#include <stdio.h>

void BamBam_BamCollationTempFileGenerator_Print(FILE * file, BamBam_BamCollationTempFileGenerator const * gen)
{
	fprintf(file,"BamBam_BamCollationTempFileGenerator(prefix=%s,nextid=%llu,tempfilenames=",gen->prefix,(unsigned long long)gen->nextid);
	BamBam_List_Print(file,gen->tempfilenames);
	fprintf(file,")");
}

/* for getpid */
#include <sys/types.h>
#include <unistd.h>
              
char * BamBam_ComputeTempFileName(char const * prefix, uint64_t const id)
{
	char * sid = 0;
	char * spid = 0;
	unsigned int tempnamelen = 0;
	char * tempname = 0;
	
	sid = BamBam_FormatNumberMinLen(id,6);
	
	if ( ! sid )
		return 0;
		
	spid = BamBam_FormatNumber(getpid());
	
	if ( ! spid )
	{
		free(sid);
		return 0;
	}
	
	tempnamelen =
		strlen(prefix) +
		1 + /* _ */
		strlen(spid) +
		1 + /* _ */
		strlen(sid) +
		1   /* NUL */
		;
		
	tempname = (char *)malloc(tempnamelen);
	
	if ( ! tempname )
	{
		free(sid);
		free(spid);
		return 0;
	}
	
	snprintf(tempname,tempnamelen,"%s_%s_%s", prefix, spid, sid);
	
	free(sid);
	free(spid);
	
	return tempname;
}

void freeTempFile(void * vtmpfilename)
{
	if ( vtmpfilename )
	{
		char * tmpfilename = (char *)vtmpfilename;
		unlink(tmpfilename);
		free(tmpfilename);
	}
	else
	{
		fprintf(stderr,"freeTempFile() called for NULL pointer.\n");
	}
}

char * BamBam_BamCollationTempFileGenerator_GetNextTempFileName(BamBam_BamCollationTempFileGenerator * gen)
{
	uint64_t id;
	char * tmpfilename = 0;
	BamBam_ListNode * listnode = 0;

	id = (gen->nextid)++;

	tmpfilename = BamBam_ComputeTempFileName(gen->prefix,id);
	
	if ( ! tmpfilename )
		return 0;
	
	listnode = BamBam_ListNode_New();
	
	if ( ! listnode )
	{
		free(tmpfilename);
		return 0;
	}
	
	listnode->entry = tmpfilename;
	listnode->bamBamListFreeFunction = freeTempFile;
	listnode->bamBamListPrintFunction = BamBam_PrintStringFunction;	
	BamBam_ListNode_PushBack(gen->tempfilenames,listnode);
	
	return tmpfilename;
}

BamBam_BamCollationTempFileGenerator * BamBam_BamCollationTempFileGenerator_New(char const * prefix)
{
	BamBam_BamCollationTempFileGenerator * gen = 0;
	
	gen = (BamBam_BamCollationTempFileGenerator *)malloc(sizeof(BamBam_BamCollationTempFileGenerator));
	
	if ( ! gen )
		return 0;
		
	gen->prefix = strdup(prefix);
	
	if ( ! gen->prefix )
	{
		free(gen);
		return 0;
	}
	
	gen->nextid = 0;
	
	gen->tempfilenames = BamBam_List_New();
	
	if ( ! gen->tempfilenames )
	{
		free(gen->prefix);
		free(gen);
		return 0;
	}
	
	return gen;
}

void BamBam_BamCollationTempFileGenerator_Delete(BamBam_BamCollationTempFileGenerator * gen)
{
	if ( gen )
	{
		BamBam_List_Delete(gen->tempfilenames);
		free(gen->prefix);
		free(gen);
	}
}
