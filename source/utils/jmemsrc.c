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

/* code was taken from:
 * [How to read JPEG resource using JpegLib ?]
 * http://www.smalleranimals.com/vforum/archive/index.php?t-1922.html
 */

/* this is not a core library module, so it doesn't define JPEG_INTERNALS */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <jpeglib.h>
#include <jerror.h>

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

// this include must stay down here to avoid typedef conflicts
#include "video.h"

static u8 * RawTo4x4RGBA(u8 *src, u32 width, u32 height)
{
	u32 block, i, c, ar, gb;
	u32 len = ((width+3)>>2)*((height+3)>>2)*32*2;
	if(len%32) len += (32-len%32);
	u8 * dst = (u8 *) memalign(32, len);
	u8 * p = dst;

	for (block = 0; block < height; block += 4)
	{
		for (i = 0; i < width; i += 4)
		{
			for (c = 0; c < 4; ++c) // Alpha and Red
			{
				for (ar = 0; ar < 4; ++ar)
				{
					*p++ = 255;
					*p++ = src[((i + ar) + ((block + c) * width)) * 3];
				}
			}
			for (c = 0; c < 4; ++c) // Green and Blue
			{
				for (gb = 0; gb < 4; ++gb)
				{
					*p++ = src[(((i + gb) + ((block + c) * width)) * 3) + 1];
					*p++ = src[(((i + gb) + ((block + c) * width)) * 3) + 2];
				}
			}
		}
	}
	DCFlushRange(dst, len);
	return dst;
}

u8 * DecodeJPEG(const u8 * src, u32 len, int * width, int * height)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	int n = len;

	while (n > 1)
	{
		if (src[n - 1] == 0xff && src[n] == 0xd9) 
			break;
		n--;
	}

	jpeg_create_decompress(&cinfo);
	cinfo.err = jpeg_std_error(&jerr);
	cinfo.progress = NULL;
	jpeg_memory_src(&cinfo, src, n);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	if (cinfo.output_width > (u32)screenwidth || cinfo.output_height > (u32)screenheight)
	{
		float factor = (cinfo.output_width > cinfo.output_height) ? (1.0 * cinfo.output_width) / screenwidth : (1.0 * cinfo.output_height) / screenheight;
		cinfo.scale_num = 1;
		cinfo.scale_denom = factor;
		cinfo.do_fancy_upsampling = true;
		cinfo.do_block_smoothing = false;
		cinfo.dct_method = JDCT_IFAST;
	}

	jpeg_start_decompress(&cinfo);

	int rowsize = cinfo.output_width * cinfo.num_components;
	u8 *tmpData = (u8 *)malloc(rowsize * cinfo.output_height);

	JSAMPROW row_pointer[1];

	row_pointer[0] = (u8 *)malloc(rowsize);
	size_t location = 0;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		memcpy(tmpData + location, row_pointer[0], rowsize);
		location += rowsize;
	}

	*width = cinfo.output_width;
	*height = cinfo.output_height;
	u8 * dst = RawTo4x4RGBA(tmpData, *width, *height);

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);
	free(tmpData);
	return dst;
}
