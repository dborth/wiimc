/****************************************************************************
 * WiiMC
 * Tantric 2009
 *
 * http.h
 *
 * HTTP operations
 ***************************************************************************/

#ifndef _HTTP_H_
#define _HTTP_H_

int http_request (char *url, FILE *hfile, char *buffer, u32 maxsize, bool silent);

#endif
