#ifndef	__FSYSLOC_H__
#define	__FSYSLOC_H__

#include	<iconv.h>

#define	FL_FSYS_SIZE	32
#define	FL_LOC_SIZE	32
#define	FL_CP_SIZE	32

typedef	struct {
	  /* the name of the filesystem */   
	char	**names;
 	  /* the locale */
	char	*locale;
	  /* codepage from when open-dir */
	char	*cpfsys;
	  /* codepage to when opend-r */
	char	*cpplayer;
	  /* the iconv handle for encoding from file-system to the player*/
	iconv_t	cdfromfsys ;
	  /* the iconv handle for encoding from player to file-system */
	iconv_t	cdtofsys ;
} fsysloc_t ;

typedef	struct {
	int	numfsys ;
	fsysloc_t fsys[1] ;
} fsysloc_table_t ;


  /**
   * initializer 
   * @param confpath - the path of the configuraiton file
   * @return - the file-system-locale-table
   */
fsysloc_table_t	*fsysloc_table_init( const char* confpath);

  /**
   * the destructor.
   * @param table - the table that was returned by fsysloc_init().
   */
void fsysloc_table_close( fsysloc_table_t *table);


  /**
   * locate the fsysloc_t object from a file fullpath
   */
const fsysloc_t *fsysloc_table_locate( const fsysloc_table_t *table, const char* fullpath);

char* fsysloc_setlocale( const fsysloc_t *fsysloc);

void fsysloc_restorelocale( const fsysloc_t *fsysloc, char* locale);

char* fsysloc_iconv_from_fsys( const fsysloc_t *fsysloc, const char* buf);

char* fsysloc_iconv_to_fsys( const fsysloc_t *fsysloc, const char* buf);


#endif

