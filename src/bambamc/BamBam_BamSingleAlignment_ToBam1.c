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
#include <bambamc/BamBam_BamSingleAlignment_ToBam1.h>

int BamBam_BamSingleAlignment_ToBam1(BamBam_BamSingleAlignment const * algn, bam1_t * bamalgn)
{
	bamalgn->core.tid = BamBam_BamSingleAlignment_GetRefId(algn);
	bamalgn->core.pos = BamBam_BamSingleAlignment_GetPos(algn);
	bamalgn->core.bin = BamBam_BamSingleAlignment_GetBin(algn);
	bamalgn->core.qual = BamBam_BamSingleAlignment_GetMQ(algn);
	bamalgn->core.l_qname = BamBam_BamSingleAlignment_GetNL(algn);
	bamalgn->core.flag = BamBam_BamSingleAlignment_GetFlags(algn);
	bamalgn->core.n_cigar = BamBam_BamSingleAlignment_GetNC(algn);
	bamalgn->core.l_qseq = BamBam_BamSingleAlignment_GetLSeq(algn);
	bamalgn->core.mtid = BamBam_BamSingleAlignment_GetNextRefID(algn);
	bamalgn->core.mpos = BamBam_BamSingleAlignment_GetNextPos(algn);
	bamalgn->core.isize = BamBam_BamSingleAlignment_GetTLen(algn);
	
	if ( bamalgn->m_data < (int64_t)algn->dataused )
	{
		free(bamalgn->data);
		bamalgn->data = 0;
		bamalgn->l_aux = 0;
		bamalgn->data_len = 0;
		bamalgn->m_data = 0;
		
		bamalgn->data = (uint8_t *)malloc(algn->dataav);
		
		if ( ! bamalgn->data )
			return -1;
		
		bamalgn->m_data = algn->dataav;
	}
	
	bamalgn->data_len = algn->dataused;
	memcpy(bamalgn->data, algn->data, bamalgn->data_len);
	bamalgn->l_aux = BamBam_BamSingleAlignment_GetAuxLength(algn);
	
	return 0;
}
