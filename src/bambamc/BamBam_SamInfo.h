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
#if ! defined(BAMBAMC_SAMINFO_H)
#define BAMBAMC_SAMINFO_H

#include <stdint.h>

typedef enum _bambamc_sam_field_status { bambamc_sam_field_undefined = 0, bambamc_sam_field_defined = 1 } bambamc_sam_field_status;
typedef char const * BamBam_SamInfo_cptrpair[2];

typedef struct _BamBam_SamInfo
{
	char * qname;
	unsigned int qnamemax;
	bambamc_sam_field_status qnamedefined;
	unsigned int qnamelen;
	
	int32_t flag;
	bambamc_sam_field_status flagdefined;		

	char * rname;
	unsigned int rnamemax;
	bambamc_sam_field_status rnamedefined;	

	int32_t pos;
	bambamc_sam_field_status posdefined;		

	int32_t mapq;
	bambamc_sam_field_status mapqdefined;

	char * cigar;
	unsigned int cigarmax;
	bambamc_sam_field_status cigardefined;	

	char * rnext;
	unsigned int rnextmax;
	bambamc_sam_field_status rnextdefined;	

	int32_t pnext;
	bambamc_sam_field_status pnextdefined;

	int32_t tlen;
	bambamc_sam_field_status tlendefined;

	char * seq;
	unsigned int seqmax;
	bambamc_sam_field_status seqdefined;	
	unsigned int seqlen;

	char * qual;
	unsigned int qualmax;
	bambamc_sam_field_status qualdefined;	
	unsigned int quallen;
} BamBam_SamInfo;

extern BamBam_SamInfo * BamBam_SamInfo_Delete(BamBam_SamInfo * object);
extern BamBam_SamInfo * BamBam_SamInfo_New();
extern void BamBam_SamInfo_initSamCharTables();
extern int BamBam_SamInfo_parseSamLine(BamBam_SamInfo_cptrpair * fields, BamBam_SamInfo * saminfo);
#endif
