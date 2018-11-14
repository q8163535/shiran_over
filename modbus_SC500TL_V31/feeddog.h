#ifndef  FEED_H__
#define  FEED_H__

/*进程可使用信号*/
typedef enum
{
	SIGUSER0=35,
	SIGUSER1,
	SIGUSER2,
	SIGUSER3,
	SIGUSER4,
	
}SIGNAL;

void sw_watchdog_init();
void sw_watchdog_feed(int signo);

#endif
