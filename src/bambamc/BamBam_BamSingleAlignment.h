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
#include <bambamc/BamBam_GzipWriter.h>
#include <bambamc/BamBam_BgzfCompressor.h>
#include <string.h>

typedef enum _bambamc_alignment_validity {
	bambamc_alignment_validity_ok = 0,
	bambamc_alignment_validity_block_too_small = 1,
	bambamc_alignment_validity_queryname_extends_over_block = 2,
	bambamc_alignment_validity_queryname_length_inconsistent = 3,
	bambamc_alignment_validity_cigar_extends_over_block = 4,
	bambamc_alignment_validity_sequence_extends_over_block = 5,
	bambamc_alignment_validity_quality_extends_over_block = 6,
	bambamc_alignment_validity_cigar_is_inconsistent_with_sequence_length = 7,
	bambamc_alignment_validity_unknown_cigar_op = 8,
	bambamc_alignment_validity_queryname_contains_illegal_symbols = 9,
	bambamc_alignment_validity_queryname_empty = 10,
	bambamc_alignment_validity_invalid_mapping_position = 11,
	bambamc_alignment_validity_invalid_next_mapping_position = 12,
	bambamc_alignment_validity_invalid_tlen = 13,
	bambamc_alignment_validity_invalid_quality_value = 14,
	bambamc_alignment_validity_invalid_refseq = 15,
	bambamc_alignment_validity_invalid_next_refseq = 16,
	bambamc_alignment_validity_invalid_auxiliary_data = 17,
	bambamc_alignment_valididy_record_length_negative = 18
} bambamc_alignment_validity;


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
	
	bambamc_alignment_validity valid;
	
	BamBam_CharBuffer * auxbuffer;
}
BamBam_BamSingleAlignment;

extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_Delete(BamBam_BamSingleAlignment * data);
extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_New();
extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_NewClone(uint8_t const * block, uint32_t const blocksize);
extern BamBam_BamSingleAlignment * BamBam_BamSingleAlignment_Clone(BamBam_BamSingleAlignment const * o);
extern int BamBam_BamSingleAlignment_LoadAlignment(BamBam_BamSingleAlignment * data, BamBam_GzipReader * reader);
extern int BamBam_BamSingleAlignment_StoreAlignment(BamBam_BamSingleAlignment const * data, BamBam_GzipWriter * writer);
extern int BamBam_BamSingleAlignment_StoreAlignmentBgzf(BamBam_BamSingleAlignment const * data, BamBam_BgzfCompressor * writer);

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
extern char const * BamBam_Alignment_Validity_Str(bambamc_alignment_validity const code);
#endif
