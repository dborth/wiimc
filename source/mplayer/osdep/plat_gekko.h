#ifndef __PLAT_GEKKO_H__
#define __PLAT_GEKKO_H__

#ifdef GEKKO

#include <sys/time.h>
#include <sys/dir.h>
#include <unistd.h>

#include <fat.h>

#include <gctypes.h>
#include <ogc/mutex.h>

extern bool reset_pressed;
extern bool power_pressed;
extern mutex_t watchdogmutex;
extern int watchdogcounter;

#ifndef PATH_MAX
#define PATH_MAX MAXPATHLEN
#endif


void gekko_abort(void);
bool DVDGekkoMount();

#define abort(x) gekko_abort(x)

void plat_init (int size);
void plat_deinit (int rc);

#define WATCH_TIMEOUT 5
static inline void setwatchdogcounter(int counter) // -1 disable watchdog
{
	if(watchdogmutex==LWP_MUTEX_NULL) return;
	LWP_MutexLock(watchdogmutex);
	//printf("watchdogcounter: %i\n",counter);
	watchdogcounter=counter;
	LWP_MutexUnlock(watchdogmutex);
}


#endif

#endif
