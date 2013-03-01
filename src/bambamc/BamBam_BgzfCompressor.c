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
#include <bambamc/BamBam_BgzfCompressor.h>

static const char BamBam_GzipHeaderData[18] =
{
	BAMBAMC_LZ_ID1, BAMBAMC_LZ_ID2, BAMBAMC_LZ_CM, BAMBAMC_LZ_FEXTRA,
	0,0,0,0, /* modification time, not given */
	0, /* XFL */
	BAMBAMC_LZ_OS_UNKNOWN,
	6,0, /* XLEN, number 6 as little endian */
	'B', 'C', /* extra data block tag */
	2,0, /* length of extra field, 2 as little endian */
	0,0  /* extra field: blocksize-1 including block header and footer, to be filled */
};

static unsigned int const BAMBAMC_LZ_HEADERSIZE = sizeof(BamBam_GzipHeaderData);
static unsigned int const BAMBAMC_LZ_FOOTERSIZE = 8;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int BamBam_BgzfCompressor_FlushInternal(BamBam_BgzfCompressor * object, int level)
{
	uint32_t insize = object->inbufferfill;
	z_stream strm;
	
	do
	{
		int zret = -1;
		
		memset ( &strm , 0, sizeof(z_stream) );
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		zret = deflateInit2(&strm, level, Z_DEFLATED, -15 /* window size */,  
			8 /* mem level, gzip default */, Z_DEFAULT_STRATEGY);

		if ( zret != Z_OK )
			return -1;

		strm.avail_in = insize;
		strm.next_in = object->inbuffer;                                                                 
		strm.avail_out = BAMBAMC_LZ_MAXBUFSIZE - (BAMBAMC_LZ_HEADERSIZE+BAMBAMC_LZ_FOOTERSIZE);
		strm.next_out = object->outbuffer + BAMBAMC_LZ_HEADERSIZE;

		zret = deflate(&strm,Z_FINISH);
		
		/* everything compressed, write block to file */
		if ( zret == Z_STREAM_END )
		{
			/* size of compressed data */
			uint32_t const payloadsize = (BAMBAMC_LZ_MAXBUFSIZE - (BAMBAMC_LZ_HEADERSIZE+BAMBAMC_LZ_FOOTERSIZE)) - strm.avail_out;
			/* blocksize stored in extended header */
			uint16_t const headblocksize = sizeof(BamBam_GzipHeaderData)/*header*/+8/*footer*/+payloadsize-1;
			/* remaining uncompressed bytes */
			uint32_t const rembytes = object->inbufferfill - insize;
			/* pointer to uncompressed rest */
			Bytef const * remp = object->inbuffer + insize;
			/* empty crc */
			uint32_t crc = crc32(0,0,0);
			/* nextout ptr */
			Bytef * nextout = strm.next_out;
			/* total size of compressed block */
			int32_t compsize = -1;

			deflateEnd(&strm);
			
			/* copy header into its place */
			memcpy(object->outbuffer,BamBam_GzipHeaderData,sizeof(BamBam_GzipHeaderData));
			
			/* put block size (2 byte little endian) */
			object->outbuffer[16] = (headblocksize >> 0) & 0xFFu;
			object->outbuffer[17] = (headblocksize >> 8) & 0xFFu;

			/* compute crc */
			crc = crc32(crc, object->inbuffer, insize);

			/* put crc */
			*(nextout++) = (crc >>  0) & 0xFFu;
			*(nextout++) = (crc >>  8) & 0xFFu;
			*(nextout++) = (crc >> 16) & 0xFFu;
			*(nextout++) = (crc >> 24) & 0xFFu;
			
			/* put uncompressed size */
			*(nextout++) = (insize >>  0) & 0xFFu;
			*(nextout++) = (insize >>  8) & 0xFFu;
			*(nextout++) = (insize >> 16) & 0xFFu;
			*(nextout++) = (insize >> 24) & 0xFFu;

			/* write block */
			compsize = nextout-object->outbuffer;
			if ( fwrite(object->outbuffer,compsize,1,object->file) != 1 )
				return -1;
			
			/* move uncompressed rest (if any) to start of buffer */
			if ( rembytes )
				memmove(object->inbuffer,remp,rembytes);
			object->inbufferfill = rembytes;
			
			return compsize;
		}
		/* output buffer too small for compressed stream, reduce size of input (and try again) */
		else if ( zret == Z_OK )
		{
			fprintf(stderr,"Bad compression, reducing.\n");
			deflateEnd(&strm);
			insize -= ((insize >= 1024) ? 1024 : insize);
		}
		/* other zlib error, give up */
		else
		{
			deflateEnd(&strm);
			return -1;
		}
	} while ( insize );
	
	return -1;
}

int BamBam_BgzfCompressor_Flush(BamBam_BgzfCompressor * object)
{
	while ( object->inbufferfill )
	{
		int const r = BamBam_BgzfCompressor_FlushInternal(object,object->level);
		
		if ( r < 0 )
			return -1;
	}
	
	return 0;
}

int BamBam_BgzfCompressor_Terminate(BamBam_BgzfCompressor * object)
{
	int r = -1;
	/* flush */
	r = BamBam_BgzfCompressor_Flush(object);
	
	if ( r < 0 )
		return -1;
	
	/* write empty block, default compression (EOF block) */
	r = BamBam_BgzfCompressor_FlushInternal(object,Z_DEFAULT_COMPRESSION);
	
	if ( r < 0 )
		return -1;
	
	/* flush the underlying file/stream */
	if ( fflush(object->file) != 0 )
		return -1;
		
	return 0;
}

int BamBam_BgzfCompressor_Write(BamBam_BgzfCompressor * object, uint8_t const * data, uint32_t len)
{
	/* loop until all data is in buffer */
	while ( len )
	{
		uint32_t const space = BAMBAMC_LZ_MAXBUFSIZE - object->inbufferfill;
		uint32_t const towrite = (len <= space) ? len : space;
		
		/* if there is no space, then try to flush out some data */
		if ( ! space )
		{
			int const r = BamBam_BgzfCompressor_FlushInternal(object,object->level);
			if ( r < 0 )
				return -1;
		}
		else
		{
			assert ( towrite );
			/* copy data */
			memcpy(object->inbuffer+object->inbufferfill,data,towrite);
			/* update counters */
			object->inbufferfill += towrite;
			data += towrite;
			len -= towrite;
		}
	}
	
	return 0;
}


static BamBam_BgzfCompressor * BamBam_BgzfCompressor_Setup(BamBam_BgzfCompressor * object, int const level)
{
	object->level = level;

	object->inbuffer = (Bytef *)malloc(BAMBAMC_LZ_MAXBUFSIZE);
	if ( ! object->inbuffer )
		return BamBam_BgzfCompressor_Delete(object);

	object->outbuffer = (Bytef *)malloc(BAMBAMC_LZ_MAXBUFSIZE);
	if ( ! object->outbuffer )
		return BamBam_BgzfCompressor_Delete(object);
		
	object->inbufferfill = 0;

	return object;
}

BamBam_BgzfCompressor * BamBam_BgzfCompressor_New(char const * filename, int const level)
{
	BamBam_BgzfCompressor * object = 0;
	
	object = (BamBam_BgzfCompressor *)malloc(sizeof(BamBam_BgzfCompressor));
	
	if ( ! object )
		return BamBam_BgzfCompressor_Delete(object);
		
	memset(object,0,sizeof(BamBam_BgzfCompressor));
		
	object->closefile = fopen(filename,"wb");
	
	if ( ! object->closefile )
		return BamBam_BgzfCompressor_Delete(object);
		
	object->file = object->closefile;

	return BamBam_BgzfCompressor_Setup(object,level);
}
BamBam_BgzfCompressor * BamBam_BgzfCompressor_NewFP(FILE * rfile, int const level)
{
	BamBam_BgzfCompressor * object = 0;

	object = (BamBam_BgzfCompressor *)malloc(sizeof(BamBam_BgzfCompressor));
	
	if ( ! object )
		return BamBam_BgzfCompressor_Delete(object);
		
	memset(object,0,sizeof(BamBam_BgzfCompressor));
		
	object->file = rfile;
	
	if ( ! object->file )
		return BamBam_BgzfCompressor_Delete(object);
		
	return BamBam_BgzfCompressor_Setup(object,level);
}
BamBam_BgzfCompressor * BamBam_BgzfCompressor_Delete(BamBam_BgzfCompressor * object)
{
	if ( object )
	{
		if ( object->file )
		{
			fflush(object->file);
			object->file = 0;
		}
		if ( object->closefile )
		{
			fclose(object->closefile);
			object->closefile = 0;
		}		
		if ( object->inbuffer )
		{
			free(object->inbuffer);
			object->inbuffer = 0;
		}
		if ( object->outbuffer )
		{
			free(object->outbuffer);
			object->outbuffer = 0;
		}
		free(object);
	}
	
	return 0;
}
int BamBam_BgzfCompressor_PutInt32(BamBam_BgzfCompressor * object, int32_t const v)
{
	uint8_t data[4] = { (v >> 0) & 0xFFu,  (v >> 8) & 0xFFu,  (v >> 16) & 0xFFu,  (v >> 24) & 0xFFu };
	return BamBam_BgzfCompressor_Write(object,&data[0],4);
}
int BamBam_BgzfCompressor_PutStringZ(BamBam_BgzfCompressor * object, char const * c)
{
	uint32_t const len = strlen(c);
	return BamBam_BgzfCompressor_Write(object,(uint8_t const *)c,len+1);
}
int BamBam_BgzfCompressor_PutLenStringZ(BamBam_BgzfCompressor * object, char const * c)
{
	uint32_t const len = strlen(c);
	int r = -1;
	
	/* put length */
	r = BamBam_BgzfCompressor_PutInt32(object,len+1);
	
	if ( r < 0 )
		return -1;
	
	/* put string */
	r = BamBam_BgzfCompressor_PutStringZ(object,c);
	
	if ( r < 0 )
		return -1;
		
	return 0;
}
