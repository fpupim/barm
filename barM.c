/*
 * Copyright (C) 2014,2015 levi0x0 with enhancements by ProgrammerNerd
 * 
 * barM (bar_monitor or BarMonitor) is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 *  This is a new version of bar monitor, even less lines of code more effective.
 *
 *  Read main() to configure your new status Bar.
 *
 *  compile: gcc -o barM barM.c -O2 -s -lX11
 *  
 *  mv barM /usr/local/bin/
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <X11/Xlib.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

/*
 *  Put this in your .xinitrc file: 
 *
 *  barM&
 *  
 */

#define VERSION "0.12"
#define TIME_FORMAT "%H:%M) (%d/%m/%Y"
#define DATALOCALE "/media/data/"
#define MAXSTR  1024

static const char * date(void);
static const char * ram(void);
static const char * get_freespace(void);
static void XSetRoot(const char *name);
/*Append here your functions.*/
static const char*(*const functab[])(void)={
        get_freespace, date
};

int main(void){
        char status[MAXSTR];
        /* It is foolish to repeatedly update uname. */
        int ret;
        char*off=status+ret;
        if(off>=(status+MAXSTR)){
                XSetRoot(status);
                return 1;/*This should not happen*/
        }
        for(;;){
                int left=sizeof(status)-ret,i;
                char*sta=off;
                for(i = 0; i<sizeof(functab)/sizeof(functab[0]); ++i ) {
                        int ret=snprintf(sta,left,"(%s) ",functab[i]());
                        sta+=ret;
                        left-=ret;
                        if(sta>=(status+MAXSTR))/*When snprintf has to resort to truncating a string it will return the length as if it were not truncated.*/
                                break;
                }
                XSetRoot(status);
                sleep(5);
        }
        return 0;
}

/* Returns the date*/
static const char * date(void){
        static char date[MAXSTR];
        time_t now = time(0);

        strftime(date, MAXSTR, TIME_FORMAT, localtime(&now));
        return date;
}
/* Returns free space */
static const char *get_freespace(void){
        static char free[MAXSTR];
        struct statvfs data;
        double french;

        if ( (statvfs(DATALOCALE, &data)) < 0){
                fprintf(stderr, "can't get info on disk.\n");
                return("?");
        }
                                                /* 1073741824 = 1024³ (B to GiB) */
        sprintf(free,"%.2ld",(data.f_frsize * data.f_bavail) / 1073741824);
        return strcat(free,"GiB");
}

/* Returns a string that contains the amount of free and available ram in megabytes*/
static const char * ram(void){
        static char ram[MAXSTR];
        struct sysinfo s;
        sysinfo(&s);
        snprintf(ram,sizeof(ram),"%.1fM,%.1fM",((double)(s.totalram-s.freeram))/1048576.,((double)s.totalram)/1048576.);
        return ram;
}

static void XSetRoot(const char *name){
        Display *display;

        if (( display = XOpenDisplay(0x0)) == NULL ) {
                fprintf(stderr, "[barM] cannot open display!\n");
                exit(1);
        }

        XStoreName(display, DefaultRootWindow(display), name);
        XSync(display, 0);

        XCloseDisplay(display);
}

