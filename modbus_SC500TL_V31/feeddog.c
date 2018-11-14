#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "feeddog.h"



static int sw_watchdog_pid=0;

void sw_watchdog_init()
{    
    FILE * fp;
    char buf_pid[10]={0};
    fp=popen("pidof app_watchdog","r");
    if((fp != NULL) && ((int32_t)fp != -1))
    {
        fgets(buf_pid,sizeof(buf_pid),fp);
        pclose(fp);

        sw_watchdog_pid=atoi(buf_pid);
        if(sw_watchdog_pid > 0)
        {
            fprintf(stderr, "sw_watchdog_pid  is %d\n",sw_watchdog_pid);

            sw_watchdog_feed(SIGUSER1);
        }
        else
        {
            sw_watchdog_pid = 0;
        }
    }
    else 
    {
        perror("daemon no exist\n");
    }
}

void sw_watchdog_feed(int signo)// 1 
{
    if(sw_watchdog_pid > 0)
    {
        kill(sw_watchdog_pid,signo);
    }
    else
    {
        sw_watchdog_init();
    }
}

