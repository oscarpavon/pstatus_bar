#include <pulse/context.h>
#include <pulse/mainloop.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/stat.h>

#include <X11/Xlib.h>

#include "pulse.h"

char *tzasuncion = "America/Asuncion";
char *tzutc = "UTC";

static Display *dpy;

#define NULL ((void*)0)

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

void
settz(char *tzname)
{
	setenv("TZ", tzname, 1);
}

char *
mktimes(char *fmt, char *tzname)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	settz(tzname);
	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL)
		return smprintf("");

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		return smprintf("");
	}

	return smprintf("%s", buf);
}

void
setstatus(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char *
execscript(char *cmd)
{
	FILE *fp;
	char retval[1025], *rv;

	memset(retval, 0, sizeof(retval));

	fp = popen(cmd, "r");
	if (fp == NULL)
		return smprintf("");

	rv = fgets(retval, sizeof(retval), fp);
	pclose(fp);
	if (rv == NULL)
		return smprintf("");
	retval[strlen(retval)-1] = '\0';

	return smprintf("%s", retval);
}

int
main(void)
{
	char *status;
	char *time_date;

	char *mic_status;
	char *getted_mic_status;
	char *screen_rec_status;



	
	pulse_init();


	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(0.5f)) {
		time_date = mktimes("%a %d %b %H:%M:%S %Y", tzasuncion);
		mic_status = "off";

		/*
		getted_mic_status = execscript("pulsemixer --id $(grep_webcam_mic_source) --get-mute");
		if(getted_mic_status[0] == '1'){
			mic_status = "Mute";
		}else {
			mic_status = "on";
		}
	*/	
		screen_rec_status = "";
		FILE* file = fopen("/tmp/screen_capture_status", "r");

		if(file != NULL){
			screen_rec_status = "REC |";
			fclose(file);
		}else{
			screen_rec_status = "";
		}

		status = smprintf("%s Mic: %s | V: %u%% %s" ,screen_rec_status , mic_status, volume_percentage,  time_date);
		setstatus(status);

		free(time_date);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}

