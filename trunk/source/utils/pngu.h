/********************************************************************************************
*
* PNGU
* 
* Original author: frontier (http://frontier-dev.net)
* Modified by Tantric, 2009-2010
*
********************************************************************************************/

#ifndef __PNGU__
#define __PNGU__

// Return codes
#define PNGU_OK							0
#define PNGU_ODD_WIDTH					1
#define PNGU_ODD_STRIDE					2
#define PNGU_INVALID_WIDTH_OR_HEIGHT	3
#define PNGU_FILE_IS_NOT_PNG			4
#define PNGU_UNSUPPORTED_COLOR_TYPE		5
#define PNGU_NO_FILE_SELECTED			6
#define PNGU_CANT_OPEN_FILE				7
#define PNGU_CANT_READ_FILE				8
#define PNGU_LIB_ERROR					9

// Color types
#define PNGU_COLOR_TYPE_GRAY			1
#define PNGU_COLOR_TYPE_GRAY_ALPHA		2
#define PNGU_COLOR_TYPE_PALETTE			3
#define PNGU_COLOR_TYPE_RGB				4
#define PNGU_COLOR_TYPE_RGB_ALPHA		5
#define PNGU_COLOR_TYPE_UNKNOWN 		6


#ifdef __cplusplus
	extern "C" {
#endif

// Types
typedef unsigned char PNGU_u8;
typedef unsigned short PNGU_u16;
typedef unsigned int PNGU_u32;
typedef unsigned long long PNGU_u64;

typedef struct
{
	PNGU_u8 r;
	PNGU_u8 g;
	PNGU_u8 b;
} PNGUCOLOR;

typedef struct
{
	PNGU_u32 imgWidth; // In pixels
	PNGU_u32 imgHeight; // In pixels
	PNGU_u32 imgBitDepth; // In bitx
	PNGU_u32 imgColorType; // PNGU_COLOR_TYPE_*
	PNGU_u32 validBckgrnd; // Non zero if there is a background color
	PNGUCOLOR bckgrnd; // Backgroun color
	PNGU_u32 numTrans; // Number of transparent colors
	PNGUCOLOR *trans; // Transparent colors
} PNGUPROP;

// Image context, always initialize with SelectImageFrom* and free with ReleaseImageContext
struct _IMGCTX;
typedef struct _IMGCTX *IMGCTX; 

/****************************************************************************
* Image context handling							                        *
****************************************************************************/

// Selects a PNG file, previosly loaded into a buffer, and creates an image context for subsequent procesing.
IMGCTX PNGU_SelectImageFromBuffer (const void *buffer);

// Selects a PNG file, from any devoptab device, and creates an image context for subsequent procesing.
IMGCTX PNGU_SelectImageFromDevice (const char *filename);

// Frees resources associated with an image context. Always call this function when you no longer need the IMGCTX.
void PNGU_ReleaseImageContext (IMGCTX ctx);

/****************************************************************************
* Miscellaneous								                             	*
****************************************************************************/

// Retrieves info from selected PNG file, including image dimensions, color format, background and transparency colors.
int PNGU_GetImageProperties (IMGCTX ctx, PNGUPROP *fileproperties);

/****************************************************************************
* Image conversion								                            *
****************************************************************************/

PNGU_u8 * DecodePNG(const PNGU_u8 *src, int *width, int *height);
int PNGU_EncodeFromRGB (IMGCTX ctx, PNGU_u32 width, PNGU_u32 height, void *buffer, PNGU_u32 stride);
int PNGU_EncodeFromGXTexture (IMGCTX ctx, PNGU_u32 width, PNGU_u32 height, void *buffer, PNGU_u32 stride);

#ifdef __cplusplus
	}
#endif

#endif
