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
#if ! defined(BAMBAM_BAMSINGLEALIGNMENT_H)
#define BAMBAM_BAMSINGLEALIGNMENT_H

#include <bambamc/BamBam_BamFileHeader.h>
#include <bambamc/BamBam_CharBuffer.h>
#include <string.h>

typedef struct _BamBam_BamSingleAlignment
{
	uint8_t * data;
	uint32_t dataav;
	uint32_t dataused;
	
	char * query;
	int32_t queryspace;

	char * qual;
	int32_t qualspace;
	
	char * cigar;
	int32_t cigarspace;
	
	BamBam_CharBuffer * auxbuffer;
}
BamBam_BamSingleAlignment;

extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_Delete(BamBam_BamSingleAlignment * data);
extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_New();
extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_NewClone(uint8_t const * block, uint32_t const blocksize);
extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_Clone(BamBam_BamSingleAlignment const * o);
extern int BamBam_BamSingleAlignment_LoadAlignment(BamBam_BamSingleAlignment * data, BamBam_GzipReader * reader);

extern int32_t BamBam_BamSingleAlignment_GetRefId(BamBam_BamSingleAlignment const * data);
extern int32_t BamBam_BamSingleAlignment_GetPos(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetBinMQNL(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetBin(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetMQ(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetNL(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetFlagNC(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetFlags(BamBam_BamSingleAlignment const * data);
extern uint32_t BamBam_BamSingleAlignment_GetNC(BamBam_BamSingleAlignment const * data);
extern int32_t BamBam_BamSingleAlignment_GetLSeq(BamBam_BamSingleAlignment const * data);
extern int32_t BamBam_BamSingleAlignment_GetNextRefID(BamBam_BamSingleAlignment const * data);
extern int32_t BamBam_BamSingleAlignment_GetNextPos(BamBam_BamSingleAlignment const * data);
extern int32_t BamBam_BamSingleAlignment_GetTLen(BamBam_BamSingleAlignment const * data);
extern char const * BamBam_BamSingleAlignment_GetReadName(BamBam_BamSingleAlignment const * data);
extern uint8_t const * BamBam_BamSingleAlignment_GetEncodedCigar(BamBam_BamSingleAlignment const * data);
extern uint8_t const * BamBam_BamSingleAlignment_GetEncodedQuery(BamBam_BamSingleAlignment const * data);
extern uint8_t const * BamBam_BamSingleAlignment_GetEncodedQual(BamBam_BamSingleAlignment const * data);
extern uint8_t const * BamBam_BamSingleAlignment_GetEncodedAux(BamBam_BamSingleAlignment const * data);
extern int BamBam_BamSingleAlignment_DecodeQueryRc(BamBam_BamSingleAlignment * algn, int const rc);
extern int BamBam_BamSingleAlignment_DecodeQualRc(BamBam_BamSingleAlignment * algn, int const rc);
extern int BamBam_BamSingleAlignment_DecodeCigarRc(BamBam_BamSingleAlignment * algn, int const rc);
extern int BamBam_BamSingleAlignment_DecodeQuery(BamBam_BamSingleAlignment * algn);
extern int BamBam_BamSingleAlignment_DecodeQual(BamBam_BamSingleAlignment * algn);
extern int BamBam_BamSingleAlignment_DecodeCigar(BamBam_BamSingleAlignment * algn);
extern int32_t BamBam_BamSingleAlignment_DecodeQueryQualCigarRc(
	BamBam_BamSingleAlignment * algn, int32_t rc, int32_t * querylen, int32_t * cigarlen);
extern int32_t BamBam_BamSingleAlignment_DecodeQueryQualCigar(
	BamBam_BamSingleAlignment * algn, int32_t * querylen, int32_t * cigarlen);
extern int BamBam_BamSingleAlignment_DecodeAuxSingle(BamBam_BamSingleAlignment * algn,
	uint8_t const * p, int reset);
extern char const * BamBam_BamSingleAlignment_DecodeAux(BamBam_BamSingleAlignment * algn);
extern uint8_t const * BamBam_BamSingleAlignment_FindAux(BamBam_BamSingleAlignment const * algn, char const tag[2]);
extern uint64_t BamBam_BamSingleAlignment_GetAuxLength(BamBam_BamSingleAlignment const * algn);
#endif
