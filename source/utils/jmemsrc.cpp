/*
 * jmemsrc.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains decompression data source routines for the case of
 * reading JPEG data from a memory buffer that is preloaded with the entire
 * JPEG file.  This would not seem especially useful at first sight, but
 * a number of people have asked for it.
 * This is really just a stripped-down version of jdatasrc.c.  Comparison
 * of this code with jdatasrc.c may be helpful in seeing how to make
 * custom source managers for other purposes.
 */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <jpeglib.h>
#include <jerror.h>
#include <gctypes.h>

#include "video.h"

//only texture in mem2, internal memory managed by gcc
#define jpg_malloc malloc
#define jpg_free free
#define jpg_memalign memalign

#include "mem2_manager.h"
//#define jpg_malloc mem2_malloc
//#define jpg_free mem2_free
//#define jpg_memalign mem2_memalign

/* Expanded data source object for memory input */

/**
 * \brief very low level C 'structure', used to decode jpeg file
 * Should not appear in the Doxygen supplied documentation
 */
typedef struct
{
	struct jpeg_source_mgr pub; /* public fields */
	JOCTET eoi_buffer[2]; /* a place to put a dummy EOI */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
	/* No work, since jpeg_memory_src set up the buffer pointer and count.
	 * Indeed, if we want to read multiple JPEG images from one buffer,
	 * this *must* not do anything to the pointer.
	 */
	(void)cinfo;
}

/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In this application, this routine should never be called; if it is called,
 * the decompressor has overrun the end of the input buffer, implying we
 * supplied an incomplete or corrupt JPEG datastream.  A simple error exit
 * might be the most appropriate response.
 *
 * But what we choose to do in this code is to supply dummy EOI markers
 * in order to force the decompressor to finish processing and supply
 * some sort of output image, no matter how corrupted.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;

	WARNMS(cinfo, JWRN_JPEG_EOF);

	/* Create a fake EOI marker */
	src->eoi_buffer[0] = (JOCTET) 0xFF;
	src->eoi_buffer[1] = (JOCTET) JPEG_EOI;
	src->pub.next_input_byte = src->eoi_buffer;
	src->pub.bytes_in_buffer = 2;

	return TRUE;
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * If we overrun the end of the buffer, we let fill_input_buffer deal with
 * it.  An extremely large skip could cause some time-wasting here, but
 * it really isn't supposed to happen ... and the decompressor will never
 * skip more than 64K anyway.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;

	if (num_bytes> 0)
	{
		while (num_bytes> (long) src->pub.bytes_in_buffer)
		{
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never return FALSE,
			 * so suspension need not be handled.
			 */
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
	/* no work necessary here */
	(void)cinfo;
}

/*
 * Prepare for input from a memory buffer.
 */

GLOBAL(void)
jpeg_memory_src (j_decompress_ptr cinfo, const JOCTET * buffer, size_t bufsize)
{
	my_src_ptr src;

	/* The source object is made permanent so that a series of JPEG images
	 * can be read from a single buffer by calling jpeg_memory_src
	 * only before the first one.
	 * This makes it unsafe to use this manager and a different source
	 * manager serially with the same JPEG object.  Caveat programmer.
	 */
	if (cinfo->src == NULL)
	{ /* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
		sizeof(my_source_mgr));
	}

	src = (my_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_source;

	src->pub.next_input_byte = buffer;
	src->pub.bytes_in_buffer = bufsize;
}

static inline u32 coordsRGBA8(u32 x, u32 y, u32 w)
{
	return ((((y >> 2) * (w >> 2) + (x >> 2)) << 5) + ((y & 3) << 2) + (x & 3)) << 1;
}

static u8 * RawTo4x4RGBA(u8 *src, u32 width, u32 height, u32 rowsize, u8 *dstPtr)
{
	u32 newWidth = width;
	if(newWidth%4) newWidth += (4-newWidth%4);
	u32 newHeight = height;
	if(newHeight%4) newHeight += (4-newHeight%4);

	int len = (newWidth * newHeight) << 2;
	if(len%32) len += (32-len%32);

	u8 *dst;

	if(dstPtr)
		dst = dstPtr; // use existing allocation
	else
		dst = (u8 *)mem2_memalign (32, len, "gui");

	if(!dst)
		return NULL;

	u32 x, y, offset;
	u8 *pixel;

	for (y = 0; y < newHeight; y++)
	{
		for (x = 0; x < newWidth; x++)
		{
			offset = coordsRGBA8(x, y, newWidth);

			if(x >= width || y >= height)
			{
				dst[offset] = 0;
				dst[offset+1] = 255;
				dst[offset+32] = 255;
				dst[offset+33] = 255;
			}
			else
			{
				pixel = &src[rowsize*y+x*3];
				dst[offset] = 255; // Alpha
				dst[offset+1] = pixel[0]; // Red
				dst[offset+32] = pixel[1]; // Green
				dst[offset+33] = pixel[2]; // Blue
			}
		}
	}
	DCFlushRange(dst, len);
	return dst;
}

u8 * DecodeJPEG(const u8 *src, u32 len, int *width, int *height, u8 *dstPtr)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	jpeg_create_decompress(&cinfo);
	cinfo.err = jpeg_std_error(&jerr);
	cinfo.progress = NULL;
	jpeg_memory_src(&cinfo, src, len);
	jpeg_read_header(&cinfo, TRUE);
	if(cinfo.jpeg_color_space == JCS_GRAYSCALE)
		cinfo.out_color_space = JCS_RGB; 

	jpeg_calc_output_dimensions(&cinfo);

	if (cinfo.output_width > (u32)screenwidth || cinfo.output_height > (u32)screenheight)
	{
		float factor = (1.0 * cinfo.output_width) / screenwidth;

		if(cinfo.output_height/factor > screenheight)
			factor = (1.0 * cinfo.output_height) / screenheight;

		// libjpeg only has a simple scaler (1/2, 1/4, 1/8 etc)
		if(factor > 8)
			factor = 16;
		else if(factor > 4)
			factor = 8;
		else if(factor > 2)
			factor = 4;
		else if(factor > 1)
			factor = 2;
		else
			factor = 1;

		if(factor > 1 && cinfo.output_width/factor < screenwidth && cinfo.output_height/factor < screenheight)
		{
			// try decreasing factor, while still staying below 1024x1024
			if(cinfo.output_width/(factor/2) <= 1024 && cinfo.output_height/(factor/2) <= 1024)
				factor /= 2;
		}

		cinfo.scale_num = 1;
		cinfo.scale_denom = factor;
		cinfo.do_fancy_upsampling = true;
		cinfo.do_block_smoothing = false;
		cinfo.dct_method = JDCT_IFAST;
	}

	jpeg_start_decompress(&cinfo);

	int rowsize = cinfo.output_width * cinfo.output_components;
	u8 *tmpData = (u8 *)jpg_malloc(rowsize * cinfo.output_height);
	JSAMPROW row_pointer[1];
	row_pointer[0] = (u8 *)jpg_malloc(rowsize);

	if(!tmpData || !row_pointer[0])
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	size_t location = 0;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		memcpy(tmpData + location, row_pointer[0], rowsize);
		location += rowsize;
	}

	u8 *dst = RawTo4x4RGBA(tmpData, cinfo.output_width, cinfo.output_height, rowsize, dstPtr);

	if(dst)
	{
		*width = cinfo.output_width;
		*height = cinfo.output_height;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	jpg_free(row_pointer[0]);
	jpg_free(tmpData);
	return dst;
}
