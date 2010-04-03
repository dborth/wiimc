/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

// Initial draft of my new cache system...
// Note it runs in 2 processes (using fork()), but doesn't requires locking!!
// TODO: seeking, data consistency checking

#define READ_USLEEP_TIME 10000
#define FILL_USLEEP_TIME 50000
#define PREFILL_SLEEP_TIME 200
#define CONTROL_SLEEP_TIME 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "input/input.h"
#include "osdep/shmem.h"
#include "osdep/timer.h"
#if defined(__MINGW32__)
#include <windows.h>
static void ThreadProc( void *s );
#elif defined(__OS2__)
#define INCL_DOS
#include <os2.h>
static void ThreadProc( void *s );
#elif defined(GEKKO)
#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>
static void *ThreadProc( void *s );
static unsigned char *global_buffer=NULL;
#include "osdep/mem2_manager.h"

#elif defined(PTHREAD_CACHE)
#include <pthread.h>
static void *ThreadProc(void *s);
#else
#include <sys/wait.h>
#endif
#include "mp_osd.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream.h"
#include "cache2.h"
extern int use_gui;

int stream_fill_buffer(stream_t *s);
int stream_seek_long(stream_t *s,off_t pos);

#ifdef GEKKO
#include <ogc/mutex.h>
static mutex_t cache_mutex = LWP_MUTEX_NULL;
static void *cachearg = NULL;
extern void SuspendCacheThread();
extern void ResumeCacheThread();
#endif

typedef struct {
  // constats:
  unsigned char *buffer;      // base pointer of the alllocated buffer memory
  int buffer_size; // size of the alllocated buffer memory
  int sector_size; // size of a single sector (2048/2324)
  int back_size;   // we should keep back_size amount of old bytes for backward seek
  int fill_limit;  // we should fill buffer only if space>=fill_limit
  int seek_limit;  // keep filling cache if distanse is less that seek limit
  // filler's pointers:
  int eof;
  off_t min_filepos; // buffer contain only a part of the file, from min-max pos
  off_t max_filepos;
  off_t offset;      // filepos <-> bufferpos  offset value (filepos of the buffer's first byte)
  // reader's pointers:
  off_t read_filepos;
  // commands/locking:
//  int seek_lock;   // 1 if we will seek/reset buffer, 2 if we are ready for cmd
//  int fifo_flag;  // 1 if we should use FIFO to notice cache about buffer reads.
  // callback
  stream_t* stream;
  volatile int control;
  volatile unsigned control_uint_arg;
  volatile double control_double_arg;
  volatile int control_res;
  volatile off_t control_new_pos;
  volatile double stream_time_length;
#ifdef GEKKO
  int thread_active;
  int exited;
#endif
} cache_vars_t;

static int min_fill=0;

float cache_fill_status=0;

void cache_stats(cache_vars_t* s){
  int newb=s->max_filepos-s->read_filepos; // new bytes in the buffer
  //mp_msg(MSGT_CACHE,MSGL_INFO,"0x%06X  [0x%06X]  0x%06X   ",(int)s->min_filepos,(int)s->read_filepos,(int)s->max_filepos);
  //mp_msg(MSGT_CACHE,MSGL_INFO,"%3d %%  (%3d%%)\n",100*newb/s->buffer_size,100*min_fill/s->buffer_size);
}

int cache_read(cache_vars_t* s,unsigned char* buf,int size){
  int total=0;
  
  while(size>0 ){
    int pos,newb,len;

  //printf("CACHE2_READ: 0x%X <= 0x%X <= 0x%X  \n",s->min_filepos,s->read_filepos,s->max_filepos);

    if(s->read_filepos>=s->max_filepos || s->read_filepos<s->min_filepos){
	// eof?
	if(s->eof) 
	{
		//if(LWP_MutexTryLock(cache_mutex))LWP_MutexUnlock(cache_mutex);
		return total;
	} 

	// waiting for buffer fill...
	//LWP_MutexUnlock(cache_mutex);
	usec_sleep(READ_USLEEP_TIME); // 10ms
	//LWP_MutexLock(cache_mutex);
	continue; // try again...
    }	
    newb=s->max_filepos-s->read_filepos; // new bytes in the buffer
    if(newb<min_fill) min_fill=newb; // statistics...

//    printf("*** newb: %d bytes ***\n",newb);

    pos=s->read_filepos - s->offset;
    if(pos<0) pos+=s->buffer_size; else
    if(pos>=s->buffer_size) pos-=s->buffer_size;

    if(newb>s->buffer_size-pos) newb=s->buffer_size-pos; // handle wrap...
    if(newb>size) newb=size;

    // check:
    if(s->read_filepos<s->min_filepos) 
	{
		mp_msg(MSGT_CACHE,MSGL_ERR,"Ehh. s->read_filepos<s->min_filepos !!! Report bug...\n");
		//debug_str="Ehh. s->read_filepos<s->min_filepos !!! Report bug...\n";
	}

    // len=write(mem,newb)
    //printf("Buffer read: %d bytes\n",newb);
    
	if(newb<=0 || pos<0 || pos+newb > s->buffer_size) // very very odd error
    {
    	//debug_str="Ehh. very very odd error !!! Report bug...\n";
    	continue;
	}
    memcpy(buf,&s->buffer[pos],newb);
    buf+=newb;
    len=newb;
    // ...

    s->read_filepos+=len;
    size-=len;
    total+=len;

  }
  //cache_fill_status=(s->max_filepos-s->read_filepos)/(s->buffer_size / 100);
  return total;
}

int cache_fill(cache_vars_t* s){
  int back,back2,newb,space,len,pos;
  off_t read;

retry:
  
  if(s->eof) 
  {
      cache_fill_status=-1;
	  return 0;
  }  
  read=s->read_filepos;
  if(read<s->min_filepos || read>s->max_filepos){
      // seek...
      //mp_msg(MSGT_CACHE,MSGL_DBG2,"Out of boundaries... seeking to 0x%"PRIX64"  \n",(int64_t)read);
      // streaming: drop cache contents only if seeking backward or too much fwd:
      if(s->stream->type!=STREAMTYPE_STREAM ||
          read<s->min_filepos || read>=s->max_filepos+s->seek_limit)
      {
        s->offset= // FIXME!?
        s->min_filepos=s->max_filepos=read; // drop cache content :(
        //debug_str="cache_fill:drop cache content";
        if(s->stream->eof) stream_reset(s->stream);
        stream_seek(s->stream,read);
        //debug_str="cache_fill:stream_seek: ok";
        //mp_msg(MSGT_CACHE,MSGL_DBG2,"Seek done. new pos: 0x%"PRIX64"  \n",(int64_t)stream_tell(s->stream));
      }
  }

  // calc number of back-bytes:
  back=read - s->min_filepos;
  if(back<0) back=0; // strange...
  if(back>s->back_size) back=s->back_size;

  // calc number of new bytes:
  newb=s->max_filepos - read;
  if(newb<0) newb=0; // strange...

  // calc free buffer space:
  space=s->buffer_size - (newb+back);

  if(space<s->fill_limit){
//    printf("Buffer is full (%d bytes free, limit: %d)\n",space,s->fill_limit);
	  if(s->eof) cache_fill_status=-1;
  	else cache_fill_status=(s->max_filepos-s->read_filepos)*100.0/s->buffer_size;

    return 0; // no fill...
  }
  // calc bufferpos:
  pos=s->max_filepos - s->offset;
  if(pos>=s->buffer_size) pos-=s->buffer_size; // wrap-around


//  printf("### read=0x%X  back=%d  newb=%d  space=%d  pos=%d\n",read,back,newb,space,pos);

  // reduce space if needed:
  if(space>s->buffer_size-pos) space=s->buffer_size-pos;

//  if(space>32768) space=32768; // limit one-time block size
  if(space>4*s->sector_size) space=4*s->sector_size;

//  if(s->seek_lock) return 0; // FIXME

#if 1
  // back+newb+space <= buffer_size
  back2=s->buffer_size-(space+newb); // max back size
  if(s->min_filepos<(read-back2)) s->min_filepos=read-back2;
#else
  s->min_filepos=read-back; // avoid seeking-back to temp area...
#endif

  // ....
  //printf("Buffer fill: %d bytes of %d\n",space,s->buffer_size);
  //len=stream_fill_buffer(s->stream);
  //memcpy(&s->buffer[pos],s->stream->buffer,len); // avoid this extra copy!
  // ....
  if(pos<0 || space<=0 || pos+space>s->buffer_size)
  {
  	//debug_str="cache_fill: strange error";
  	//goto retry;
  }
  len=stream_read(s->stream,&s->buffer[pos],space);
  if(len==0) 
  {
  	if(s->stream->error>0)
	{
		s->stream->error++; //count read error
		
		if(s->stream->error>1000) //num retries
		{
			//s->stream->error=0;
			s->eof=1;
			//printf("eof\n");
		}
		else		
		{
		  //printf("Error reading stream\n");
		  //retry if we have cache
		  cache_fill_status=(s->max_filepos-s->read_filepos)*100.0/s->buffer_size;
		  if(cache_fill_status<5)
		  {	  
	  		s->eof=1;
	  		cache_fill_status=-1;  		
	  		//printf("error: %i\n",s->stream->error);
	  	  }
	  	  else 
			{
			s->stream->eof=0;
			//printf("retry read (%f)\n",cache_fill_status);
			}
  	    }
	}
  	else
  	{
  		cache_fill_status=-1;  	
  		s->eof=1;
  	}
  }

  s->max_filepos+=len;
  if(pos+len>=s->buffer_size){
      // wrap...
      s->offset+=s->buffer_size;
  }

  if(s->eof) cache_fill_status=-1;
  else cache_fill_status=(s->max_filepos-s->read_filepos)*100.0/s->buffer_size;

  return len;

}

static int cache_execute_control(cache_vars_t *s) {
  int res = 1;
  //static unsigned last;
  //unsigned now;
  static u64 last;
  u64 now;
  if (!s->stream->control) {
    s->stream_time_length = 0;
    s->control_new_pos = 0;
    s->control_res = STREAM_UNSUPPORTED;
    s->control = -1;
    return res;
  }
  //if(s->stream->type==STREAMTYPE_DVD || s->stream->type==STREAMTYPE_DVDNAV)
  {
	  now = GetTimerMS();
	  if (now - last > 99) {
	    double len;
	    if (s->stream->control(s->stream, STREAM_CTRL_GET_TIME_LENGTH, &len) == STREAM_OK)
	      s->stream_time_length = len;
	    else
	      s->stream_time_length = 0;
	    last = now;
	  }
  }
  if (s->control == -1) return res;
  switch (s->control) {
    case STREAM_CTRL_GET_CURRENT_TIME:
    case STREAM_CTRL_SEEK_TO_TIME:
    case STREAM_CTRL_GET_ASPECT_RATIO:
      s->control_res = s->stream->control(s->stream, s->control, &s->control_double_arg);
      break;
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    case STREAM_CTRL_GET_NUM_ANGLES:
    case STREAM_CTRL_GET_ANGLE:
    case STREAM_CTRL_SET_ANGLE:
      s->control_res = s->stream->control(s->stream, s->control, &s->control_uint_arg);
      break;
    case -2:
      res = 0;
    default:
      s->control_res = STREAM_UNSUPPORTED;
      break;
  }
  s->control_new_pos = s->stream->pos;
  s->control = -1;
  return res;
}

static cache_vars_t* cache_init(int size,int sector){
  int num;
#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__) && !defined(GEKKO)
  cache_vars_t* s=shmem_alloc(sizeof(cache_vars_t));
#else
  cache_vars_t* s=malloc(sizeof(cache_vars_t));
#endif
  if(s==NULL) return NULL;

  memset(s,0,sizeof(cache_vars_t));
  num=size/sector;
  if(num < 16){
     num = 16;
  }//32kb min_size
  s->buffer_size=num*sector;
  s->sector_size=sector;
#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__) && !defined(GEKKO)
  s->buffer=shmem_alloc(s->buffer_size);
#else
  if(global_buffer==NULL) global_buffer=mem2_malloc(size);
  s->buffer=global_buffer;
#endif

  if(s->buffer == NULL){
#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__) && !defined(GEKKO)
    shmem_free(s,sizeof(cache_vars_t));
#else
    free(s);
#endif
    return NULL;
  }
  s->fill_limit=8*sector;
#if defined(GEKKO)
  s->back_size=s->buffer_size/4; // 1/4 back  3/4 forward
#else
  s->back_size=s->buffer_size/2;
#endif
  return s;
}

void cache_uninit(stream_t *s) {
  cache_vars_t* c = s->cache_data;
  
#if defined(GEKKO)
  if(!s->cache_pid) return; 
  cache_do_control(s, -2, NULL);
  c->thread_active = 0;
  s->cache_pid = 0;
#else  
  if(s->cache_pid) {
#if defined(__MINGW32__) || defined(PTHREAD_CACHE) || defined(__OS2__)
    cache_do_control(s, -2, NULL);
#else
    kill(s->cache_pid,SIGKILL);
    waitpid(s->cache_pid,NULL,0);
#endif
	s->cache_pid = 0;
  }
#endif //GEKKO

if(!c) return;
#if defined(GEKKO)
  c->buffer=NULL;
  free(s->cache_data);
  s->cache_data=NULL;
  s->cache_pid=0;
#else
#if defined(__MINGW32__) || defined(PTHREAD_CACHE) || defined(__OS2__)
  free(c->stream);
  free(c->buffer);
  c->buffer = NULL;
  free(s->cache_data);
#else
  shmem_free(c->buffer,c->buffer_size);
  c->buffer = NULL;
  shmem_free(s->cache_data,sizeof(cache_vars_t));
#endif
  s->cache_data = NULL;
#endif //GEKKO
}


static void exit_sighandler(int x){
  // close stream
  exit(0);
}

/**
 * \return 1 on success, 0 if the function was interrupted and -1 on error
 */
int stream_enable_cache(stream_t *stream,int size,int min,int seek_limit){
  int ss = stream->sector_size ? stream->sector_size : STREAM_BUFFER_SIZE;
  int res = -1;
  cache_vars_t* s;

	cache_fill_status=-1;

  if (stream->flags & STREAM_NON_CACHEABLE) {
    //mp_msg(MSGT_CACHE,MSGL_STATUS,"\rThis stream is non-cacheable\n");
    return 1;
  }
  if(cache_mutex == LWP_MUTEX_NULL) LWP_MutexInit(&cache_mutex, false);	
/*  
if(size>CACHE_LIMIT)
{ //limit cache 
	float factor;
	factor=(float)size/CACHE_LIMIT;
	min=min/factor;
	seek_limit=seek_limit/factor;
	size=CACHE_LIMIT; 
}
*/
  s=cache_init(size,ss);
  if(s == NULL) return -1;
  stream->cache_data=s;
  s->stream=stream; // callback
  s->seek_limit=seek_limit;
  s->stream->error=0;


  //make sure that we won't wait from cache_fill
  //more data than it is alowed to fill
  if (s->seek_limit > s->buffer_size - s->fill_limit ){
     s->seek_limit = s->buffer_size - s->fill_limit;
  }
  if (min > s->buffer_size - s->fill_limit) {
     min = s->buffer_size - s->fill_limit;
  }

#if !defined(__MINGW32__) && !defined(PTHREAD_CACHE) && !defined(__OS2__) && !defined(GEKKO)
  if((stream->cache_pid=fork())){
    if ((pid_t)stream->cache_pid == -1)
      stream->cache_pid = 0;
#else
  {
    stream_t* stream2=malloc(sizeof(stream_t));
    memcpy(stream2,s->stream,sizeof(stream_t));
    s->stream=stream2;    
#if defined(__MINGW32__)
    stream->cache_pid = _beginthread( ThreadProc, 0, s );
#elif defined(GEKKO)
	s->exited=0;
	s->thread_active = 1;
	cachearg = s;
	ResumeCacheThread();
	stream->cache_pid = 1;
#elif defined(__OS2__)
    stream->cache_pid = _beginthread( ThreadProc, NULL, 256 * 1024, s );
#else
    {
    pthread_t tid;
    pthread_create(&tid, NULL, ThreadProc, s);
    stream->cache_pid = 1;
    }
#endif
#endif
    if (!stream->cache_pid) {
        mp_msg(MSGT_CACHE, MSGL_ERR,
               "Starting cache process/thread failed: %s.\n", strerror(errno));
        goto err_out;
    }
    // wait until cache is filled at least prefill_init %
    mp_msg(MSGT_CACHE,MSGL_V,"CACHE_PRE_INIT: %"PRId64" [%"PRId64"] %"PRId64"  pre:%d  eof:%d  \n",
	(int64_t)s->min_filepos,(int64_t)s->read_filepos,(int64_t)s->max_filepos,min,s->eof);
    while(s->read_filepos<s->min_filepos || s->max_filepos-s->read_filepos<min){
	mp_msg(MSGT_CACHE,MSGL_STATUS,MSGTR_CacheFill,
	    100.0*(float)(s->max_filepos-s->read_filepos)/(float)(s->buffer_size),
	    (int64_t)s->max_filepos-s->read_filepos );

	//set_osd_msg(OSD_MSG_TEXT, 1, 2000, "Precache fill: %5.2f%%  ",(float)(100.0*(float)(s->max_filepos)/(float)(min)));
	//force_osd();
	
	if(s->eof) break; // file is smaller than prefill size
	if(stream_check_interrupt(PREFILL_SLEEP_TIME)) {
	  res = 0;
	  goto err_out;
        }
    }
    mp_msg(MSGT_CACHE,MSGL_STATUS,"\n");
    return 1; // parent exits

err_out:
    cache_uninit(stream);
    return res;
  }

#if defined(__MINGW32__) || defined(PTHREAD_CACHE) || defined(__OS2__)
}
#ifdef PTHREAD_CACHE || defined(GEKKO)
static void *ThreadProc( void *s ){
#else
static void ThreadProc( void *s ){
#endif
#endif

#ifdef CONFIG_GUI
  use_gui = 0; // mp_msg may not use gui stuff in forked code
#endif
// cache thread mainloop:
#ifndef GEKKO
  signal(SIGTERM,exit_sighandler); // kill
#endif  
  do {
    if(!cache_fill(s)){
	 usec_sleep(FILL_USLEEP_TIME); // idle
    }else usleep(50);
//	 cache_stats(s->cache_data);
#ifndef GEKKO
  } while (cache_execute_control(s));
#else
  } while (cache_execute_control(s) && ((cache_vars_t*)s)->thread_active);
  ((cache_vars_t*)s)->exited=1;
#endif  
#if defined(__MINGW32__) || defined(__OS2__)
  _endthread();
#endif
#if defined(PTHREAD_CACHE) || defined(GEKKO) 
  return NULL;
#endif
  // make sure forked code never leaves this function
  exit(0);
}

#ifdef GEKKO
void *mplayercachethread(void *arg)
{
	while(1)
	{
		SuspendCacheThread();
		ThreadProc(cachearg);
	}
	return NULL;
}
#endif

int cache_stream_fill_buffer(stream_t *s){
  int len;
  if(s->eof){ s->buf_pos=s->buf_len=0; return 0; }
  if(!s->cache_pid) return stream_fill_buffer(s);

//  cache_stats(s->cache_data);

  if(s->pos!=((cache_vars_t*)s->cache_data)->read_filepos) 
  {
  	//mp_msg(MSGT_CACHE,MSGL_ERR,"!!! read_filepos differs!!! report this bug...\n");
  	//reset cache
    ((cache_vars_t*)s->cache_data)->offset = ((cache_vars_t*)s->cache_data)->min_filepos=((cache_vars_t*)s->cache_data)->max_filepos=((cache_vars_t*)s->cache_data)->read_filepos; // drop cache content :(
    if(((cache_vars_t*)s->cache_data)->stream->eof) stream_reset(s);
	s->pos!=((cache_vars_t*)s->cache_data)->read_filepos;
  }

  len=cache_read(s->cache_data,s->buffer, ((cache_vars_t*)s->cache_data)->sector_size);
  //printf("cache_stream_fill_buffer->read -> %d\n",len);

  if(len<=0){ s->eof=1; s->buf_pos=s->buf_len=0; return 0; }
  s->buf_pos=0;
  s->buf_len=len;
  s->pos+=len;
//  printf("[%d]",len);fflush(stdout);
  return len;

}

int cache_stream_seek_long(stream_t *stream,off_t pos){
  cache_vars_t* s;
  off_t newpos;
  if(!stream->cache_pid) 
  {
  	return stream_seek_long(stream,pos);
  }
  LWP_MutexLock(cache_mutex);

  s=stream->cache_data;
//  s->seek_lock=1;

//  mp_msg(MSGT_CACHE,MSGL_DBG2,"CACHE2_SEEK: 0x%"PRIX64" <= 0x%"PRIX64" (0x%"PRIX64") <= 0x%"PRIX64"  \n",s->min_filepos,pos,s->read_filepos,s->max_filepos);

  newpos=pos/s->sector_size; newpos*=s->sector_size; // align
  stream->pos=s->read_filepos=newpos;
  s->eof=0; // !!!!!!!

	LWP_MutexUnlock(cache_mutex);
  cache_stream_fill_buffer(stream);
	LWP_MutexLock(cache_mutex);

  pos-=newpos;
  //GetRelativeTime();
  if(pos>=0 && pos<=stream->buf_len){
    stream->buf_pos=pos; // byte position in sector
    LWP_MutexUnlock(cache_mutex);
    return 1;
  }
	LWP_MutexUnlock(cache_mutex);
//  stream->buf_pos=stream->buf_len=0;
//  return 1;

//  mp_msg(MSGT_CACHE,MSGL_V,"cache_stream_seek: WARNING! Can't seek to 0x%"PRIX64" !\n",(int64_t)(pos+newpos));
  return 0;
}

int cache_do_control(stream_t *stream, int cmd, void *arg) {
  cache_vars_t* s = stream->cache_data;
  switch (cmd) {
    case STREAM_CTRL_SEEK_TO_TIME:
      s->control_double_arg = *(double *)arg;
      s->control = cmd;
      break;
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    case STREAM_CTRL_SET_ANGLE:
      s->control_uint_arg = *(unsigned *)arg;
      s->control = cmd;
      break;
// the core might call these every frame, they are too slow for this...
    case STREAM_CTRL_GET_TIME_LENGTH:
//    case STREAM_CTRL_GET_CURRENT_TIME:
      *(double *)arg = s->stream_time_length;
      return s->stream_time_length ? STREAM_OK : STREAM_UNSUPPORTED;
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    case STREAM_CTRL_GET_ASPECT_RATIO:
    case STREAM_CTRL_GET_NUM_ANGLES:
    case STREAM_CTRL_GET_ANGLE:
    case -2:
      s->control = cmd;
      break;
    default:
      return STREAM_UNSUPPORTED;
  }
  
while (s->control != -1)
	usec_sleep(CONTROL_SLEEP_TIME); 

  switch (cmd) {
    case STREAM_CTRL_GET_TIME_LENGTH:
    case STREAM_CTRL_GET_CURRENT_TIME:
    case STREAM_CTRL_GET_ASPECT_RATIO:
      *(double *)arg = s->control_double_arg;
      break;
    case STREAM_CTRL_GET_NUM_CHAPTERS:
    case STREAM_CTRL_GET_CURRENT_CHAPTER:
    case STREAM_CTRL_GET_NUM_ANGLES:
    case STREAM_CTRL_GET_ANGLE:
      *(unsigned *)arg = s->control_uint_arg;
      break;
    case STREAM_CTRL_SEEK_TO_CHAPTER:
    case STREAM_CTRL_SEEK_TO_TIME:
    case STREAM_CTRL_SET_ANGLE:
      stream->pos = s->read_filepos = s->control_new_pos;
      break;
  }
  return s->control_res;
}

int stream_read(stream_t *s,char* mem,int total){
  int len=total;
  while(len>0){
    int x;
    //debug_str="stream_read";
    
    
    if(s->buf_len-s->buf_pos==0){
    	//debug_str="stream_read: cache_stream_fill_buffer";
      if(!cache_stream_fill_buffer(s)) 
	  {
	  	//debug_str="stream_read: cache_stream_fill_buffer ok return";
	  	return total-len; // EOF or error
	  }
      //debug_str="stream_read: cache_stream_fill_buffer ok";
      x=s->buf_len-s->buf_pos;
    } 
    LWP_MutexLock(cache_mutex);
    x=s->buf_len-s->buf_pos;
    if(x>len) x=len;
    /*
    if(s->buf_pos+x>s->buf_len) 
	{
			//debug_str="stream_read: WARNING! s->buf_pos>s->buf_len\n";
			usleep(10);
//			LWP_MutexUnlock(cache_mutex);
			usleep(10);
			log_console_enable_video(true);
			printf("cache er1!!\n");
			LWP_MutexUnlock(cache_mutex);
			sleep(3);
			
		return total-len;
		
	}
	*/
	//debug_str="stream_read: memcpy";
    memcpy(mem,&s->buffer[s->buf_pos],x);
	//debug_str="stream_read: ok memcpy";
    s->buf_pos+=x; mem+=x; len-=x;
    LWP_MutexUnlock(cache_mutex);
  }
  
  //debug_str="stream_read: ok";
  return total;
}

void refillcache(stream_t *stream,float min)
{
	cache_vars_t* s;
	int out=0;
	s=stream->cache_data;
	u64 t1;
	float old=0;
	t1 = GetTimerMS();

    while(cache_fill_status<min)
    {
		//set_osd_msg(OSD_MSG_TEXT, 1, 2000, "Cache fill: %5.2f%%  ",(float)(100.0*(float)(cache_fill_status)/(float)(min)));
		//force_osd();
		//printf("Cache fill: %5.2f%%  \n",(float)(100.0*(float)(cache_fill_status)/(float)(min)));
    	ShowProgress("Buffering...", (int)cache_fill_status*10000, (int)min*10000);
		if(s->eof) break; // file is smaller than prefill size
			
		if(out==0)out=stream_check_interrupt(PREFILL_SLEEP_TIME);
		else
		{ //remove others pause commands if you press pause several times
		  mp_cmd_t* cmd;
		  if((cmd = mp_input_get_cmd(PREFILL_SLEEP_TIME,0,1)) != NULL)
		  {
			  if(cmd->id==MP_CMD_PAUSE)
			  {
				  cmd = mp_input_get_cmd(time,0,0);
				  mp_cmd_free(cmd);
			  }
		  }

		}
		//printf("Cache fill: %5.2f%%  \n",cache_fill_status);
		if(cache_fill_status > 5 && out)
		{
			//printf("break Cache fill: %5.2f%%  \n",cache_fill_status);
			return ;
		}	
		
		//not needed, for security	
		if(old<cache_fill_status)t1 = GetTimerMS();
	    if(GetTimerMS()-t1>1500) return ;
		old=cache_fill_status;
		usleep(50);
    }
    //printf("end Cache fill: %5.2f%%  \n",cache_fill_status);
    
}
int stream_error(stream_t *stream)
{
	//cache_vars_t* s;

	//s=stream->cache_data;
  	return ((cache_vars_t*)stream->cache_data)->stream->error;
}
