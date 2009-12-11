/* Imported from the dvbstream project
 *
 * Modified for use with MPlayer, for details see the changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: rtp.h 26061 2008-02-22 09:09:46Z diego $
 */

#ifndef MPLAYER_RTP_H
#define MPLAYER_RTP_H

int read_rtp_from_server(int fd, char *buffer, int length);

#endif /* MPLAYER_RTP_H */
