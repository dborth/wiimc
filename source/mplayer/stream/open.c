
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <locale.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#ifdef __FreeBSD__
#include <sys/cdrio.h>
#endif

#include "m_option.h"
#include "stream.h"
#include "libmpdemux/demuxer.h"
#include "libmenu/fsysloc.h"



/// We keep these 2 for the gui atm, but they will be removed.
int vcd_track=0;
char* cdrom_device=NULL;
int dvd_chapter=1;
int dvd_last_chapter=0;
char* dvd_device=NULL;
int dvd_title=0;

// Open a new stream  (stdin/file/vcd/url)

stream_t* open_stream(char* filename,char** options, int* file_format){
   stream_t* ret;
   extern fsysloc_table_t *fsysloc_table;
   char *locale_changed = NULL ;
   const fsysloc_t *fsysloc = NULL;
   char* filename_iconv = NULL;

  // Check if playlist or unknown
  if (*file_format != DEMUXER_TYPE_PLAYLIST){
    *file_format=DEMUXER_TYPE_UNKNOWN;
  }

if(!filename) {
   mp_msg(MSGT_OPEN,MSGL_ERR,"NULL filename, report this bug\n");
   return NULL;
}

//============ Open STDIN or plain FILE ============


  fsysloc = fsysloc_table_locate( fsysloc_table, filename);
  locale_changed = fsysloc_setlocale( fsysloc);
  filename_iconv = fsysloc_iconv_to_fsys( fsysloc, filename);

  ret = open_stream_full(filename_iconv,STREAM_READ,options,file_format);

  fsysloc_restorelocale( fsysloc, locale_changed);
  if ( filename_iconv != filename ){
     free( filename_iconv);
  }
  return ret;
}

