#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#include "fsysloc.h"

static const char* skipspace( const char* p)
{
 	while( isspace((int)*p)) {
		p++;
	}
	return p;
}

static const char* getword( char *buf, int buflen, const char *p, const char* stopchars )
{
	while (*p && buflen>0) {
		if (stopchars==NULL ) {
			if ( isspace( (int)*p) ) {
				p++;
				break;
			}
		} else if ( strchr( stopchars, *p)!=NULL) {
			p++;
			break;
		}
		*buf++ = *p++;
		buflen --;
	}
	*buf = '\0' ;
	return p; 
}

fsysloc_table_t	*fsysloc_table_init( const char* confpath)
{
	FILE	*fp;
	const	char	*p;
	const	char	*r;
	char	*s;
	char	line[256];
	char	word[128];
	char	code[16];
	int	names_count;
	char	*names[16];
	char	names_buf[128];
	char	locale[32];
	char	cpfsys[32];
	char	cpplayer[32];
	int	fsys_count;
	int	i, k;
	fsysloc_table_t *fsystab = NULL;

	fp = fopen( confpath, "r");
	if ( fp==NULL) {
		return NULL;
	}

	fsys_count = 0;
	while( fgets( line, sizeof(line)-1, fp) != NULL ) {
		p = skipspace(line);

		if ( (!*p) || *p == '#' ) {
			continue;
		}
		fsys_count ++;
	}

	if ( fsys_count==0 ) {
		fclose(fp);
		return NULL;
	}
	rewind( fp);

	fsystab = ( fsysloc_table_t* ) malloc( sizeof( fsysloc_table_t)+(fsys_count-1) * sizeof( fsysloc_t));


	for( i=0; i<fsys_count; i++) {
		fsystab->fsys[i].names = NULL;
		fsystab->fsys[i].locale = NULL;
		fsystab->fsys[i].cpfsys = NULL;
		fsystab->fsys[i].cpplayer = NULL;
		fsystab->fsys[i].cdfromfsys =  (iconv_t)(-1);
		fsystab->fsys[i].cdtofsys = (iconv_t)(-1);
	}

	k = 0;

	while( fgets( line, sizeof(line)-1, fp) != NULL ) {
		p = skipspace(line);

		if ( (!*p) || *p == '#' ) {
			continue;
		}

		names_count = 0;
		locale[0] = cpfsys[0] = cpplayer[0] =  '\0';

		while ( *p ) {
			p = getword( word, sizeof(word)-1, p, NULL);

			r = getword( code, sizeof(code)-1, word, "=");
			if ( !strcmp( code, "names")) {
				getword( names_buf, sizeof(names_buf)-1, r, NULL);
				s = names_buf;
				names[0] = s;
				names_count = 1;

				while (*s) {
					if (*s==',' ) {
						*s='\0' ;

						if ( *(s+1) ) {
							names[ names_count++ ] = s+1;
						}
					}
					s++;
				}
			} else if ( !strcmp( code, "locale")) {
				getword( locale, sizeof(locale)-1, r, NULL);
			} else if ( !strcmp( code, "cpfsys")) {
				getword( cpfsys, sizeof(cpfsys)-1, r, NULL);
			} else if ( !strcmp( code, "cpplayer")) {
				getword( cpplayer, sizeof(cpplayer)-1, r, NULL);
			} 
			p = skipspace( p);
		}

		if ( names_count > 0) {
			fsystab->fsys[k].names = (char**)malloc( (names_count+1) * sizeof(char*));
			for( i=0; i<names_count; i++ ) {
				fsystab->fsys[k].names[i] = strdup( names[i]);
			}
			fsystab->fsys[k].names[i] = NULL;
			if ( locale[0] ) {
				fsystab->fsys[k].locale = strdup( locale) ;
			}
			if ( cpfsys[0] ) {
				fsystab->fsys[k].cpfsys = strdup( cpfsys) ;
			}
			if ( cpplayer[0] ) {
				fsystab->fsys[k].cpplayer = strdup( cpplayer) ;
			}

			k++;
		}
	}
	fclose(fp);

	fsystab->numfsys = k;

	for( i=0; i<k; i++) {
		if ( fsystab->fsys[i].cpfsys || fsystab->fsys[i].cpplayer) {
			const char* cpfsys = fsystab->fsys[i].cpfsys;
			const char* cpplayer = fsystab->fsys[i].cpplayer;

			if ( cpfsys==NULL ) {
				cpfsys = "UTF-8";
			}
			if ( cpplayer==NULL ) {
				cpplayer = "UTF-8";
			}
			fsystab->fsys[i].cdfromfsys = iconv_open( cpplayer,  cpfsys);
			fsystab->fsys[i].cdtofsys = iconv_open( cpfsys, cpplayer);
		}
	}

	return	fsystab;
}

  /**
   * the destructor.
   * @param table - the table that was returned by fsysloc_init().
   */
void fsysloc_table_close( fsysloc_table_t *fsystab)
{
	int	i, j;
	fsysloc_t *fsys;

	if ( fsystab==NULL ) {
		return;
	}

	fsys = fsystab->fsys;
	for( i=0; i<fsystab->numfsys; i++, fsys++) {

		if ( fsys->names ) {
			for(j=0; fsys->names[j]; j++ ) {
				free( fsys->names[j]);
			}
			free(fsys->names);
			fsys->names = NULL;
		}

		if ( fsys->locale ) {
			free(fsys->locale);
			fsys->locale = NULL;
		}

		if ( fsys->cpfsys ) {
			free(fsys->cpfsys);
			fsys->cpfsys = NULL;
		}

		if ( fsys->cpplayer ) {
			free(fsys->cpplayer);
			fsys->cpplayer = NULL;
		}

		if ( fsys->cdfromfsys != (iconv_t)(-1) ) {
			iconv_close( fsys->cdfromfsys);
			fsys->cdfromfsys =  (iconv_t)(-1);
		}

		if ( fsys->cdtofsys != (iconv_t)(-1) ) {
			iconv_close( fsys->cdtofsys);
			fsys->cdtofsys = (iconv_t)(-1);
		}
	}
	free( fsystab);
}

static int strstartwith( const char *str, const char* with)
{
	while( *with && *str) {
		if ( *with!=*str ) {
			break;
		}
		with++;
		str++;
	}

	return !*with;
}

  /**
   * locate the fsysloc_t object from a file fullpath
   */
const fsysloc_t *fsysloc_table_locate( const fsysloc_table_t *fsystab, const char* fullpath)
{
	int	i, j;
	const fsysloc_t *fsys;

	if ( fsystab==NULL ) {
		return NULL;
	}

	fsys = fsystab->fsys;
	for( i=0; i<fsystab->numfsys; i++, fsys++) {

		if ( fsys->names ) {
			for(j=0; fsys->names[j]; j++ ) {
				if ( strstartwith( fullpath, fsys->names[j])) {
					return fsys;
				}
			}
		}
	}
	return NULL;
}

char* fsysloc_setlocale( const fsysloc_t *fsysloc)
{
	char	*oldlocale = NULL;
	if ( fsysloc!=NULL) {
		if ( fsysloc->locale ) {
			oldlocale = setlocale( LC_CTYPE, NULL);
			if ( oldlocale ) {
				oldlocale = strdup( oldlocale);
			}
			setlocale( LC_CTYPE, fsysloc->locale);
		}
	}
	return oldlocale;
}

void fsysloc_restorelocale( const fsysloc_t *fsysloc, char* locale)
{
	if ( locale!=NULL) {
		//setlocale( LC_CTYPE, "C") ;
		setlocale( LC_CTYPE, locale) ;
		free( locale);
	}
}

static char* encode( iconv_t cd, const char* data, size_t size) 
{
	size_t	obufsz = size*3 + 1;
	char	*outbuf = (char*)malloc( obufsz );
	char	*op = outbuf;
	char 	*ip = (char*)data;
	size_t	osize = obufsz ;
	size_t	ileft = size;
	size_t	oleft = obufsz -1;
	size_t	rc;
	
	if ( outbuf==NULL) {
		return (char*) data;
	}

	rc = iconv( cd, &ip, &ileft, &op, &oleft);

	if ( rc==(size_t)-1) {
		free( outbuf);	
		return (char*)data;
	}
	outbuf[osize - oleft - 1] = '\0';
	return outbuf;
}

char* fsysloc_iconv_from_fsys( const fsysloc_t *fsysloc, const char* buf)
{
	if ( fsysloc==NULL || fsysloc->cdfromfsys == (iconv_t)(-1)) {
		return	(char*)buf;
	} 

	return encode( fsysloc->cdfromfsys, buf, strlen(buf));
}


char* fsysloc_iconv_to_fsys( const fsysloc_t *fsysloc, const char* buf)
{
	if ( fsysloc==NULL || fsysloc->cdtofsys == (iconv_t)(-1)) {
		return	(char*)buf;
	} 

	return encode( fsysloc->cdtofsys, buf, strlen(buf));
}

#undef	TEST_FSYSLOC
#ifdef	TEST_FSYSLOC

unsigned char	chinese[] = { 0xB5, 0xE7, 0xCA, 0xD3, 0xBE, 0xE7, 0 };


static void printbinary( const char* header, const char* bin)
{
	unsigned char *p = (unsigned char*) bin;

	printf( "%s:", header);
	while( *p) {
		printf( "%02X ", *p++);
	}
	printf( "\n");
}
int
main()
{
	fsysloc_table_t *fsystab;
	const fsysloc_t	*fsys;
	char	*oldloc;
	char	testdata[128];
	char	*outbuf;
	char	*inbuf;


	fsystab = fsysloc_table_init( "./fsysloc.conf");
	if ( fsystab==NULL) {
		fprintf( stderr, "fsysloc_table_init() error.\n");
		exit(1);
	}

	fsys = fsysloc_table_locate( fsystab, "sd:/root/whatever");

	if ( fsys==NULL) {
		fprintf( stderr, "unable to find sd:/ fsysloc_table_locate() error.\n");
		exit(1);
	}

	fsys = fsysloc_table_locate( fsystab, "usb:/root/whatever");

	if ( fsys==NULL) {
		fprintf( stderr, "unable to find usb:/ fsysloc_table_locate() error.\n");
		exit(1);
	}

	oldloc = fsysloc_setlocale( fsys);

	fsysloc_restorelocale( fsys, oldloc);


	sprintf( testdata, "smb2:/eMule/%s.rmvb", chinese);

	fsys = fsysloc_table_locate( fsystab, testdata);

	if ( fsys==NULL) {
		fprintf( stderr, "fsysloc_table_locate() error.\n");
		exit(1);
	}

	inbuf = testdata;
	outbuf = fsysloc_iconv_from_fsys( fsys, inbuf);
	printbinary( " inbuf_from_fsys:",  inbuf);
	printbinary( "outbuf_from_fsys:", outbuf);


	inbuf = outbuf;
	outbuf = fsysloc_iconv_to_fsys( fsys, inbuf);
	printbinary( "   inbuf_to_fsys:", inbuf);
	printbinary( "  outbuf_to_fsys:", outbuf);

	if ( strcmp( outbuf, testdata)==0) {
		printf( "YES ! they are equal.\n");
	} else {
		printf( "NO! continue to work....\n");
	} 
	fsysloc_table_close( fsystab);
	return 0;

}
#endif
