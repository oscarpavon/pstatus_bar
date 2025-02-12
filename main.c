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

char *tzasuncion = "America/Asuncion";
char *tzutc = "UTC";

static Display *dpy;


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
	char *screen_rec_status;

	char *available_memory;
	char *used_memory;


	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

		
	mic_status = "No";

	for (;;usleep(50000)) {
		time_date = mktimes("%a %d %b %H:%M:%S %Y", tzasuncion);
	
		char got_mic_status[2];
		FILE* mic_file = fopen("/tmp/mic", "r");
		if(mic_file){
		
			fgets(got_mic_status, 5, mic_file);
			
			if(got_mic_status[0] == '1'){
				mic_status = "Mute";
			}else{
				mic_status = "On";
			}

			fclose(mic_file);
		}


		char volume[5];
		FILE* volume_file = fopen("/tmp/volume", "r");
		if(volume_file){
			fgets(volume, 5, volume_file);
			fclose(volume_file);
		}
		
		screen_rec_status = "";
		FILE* file = fopen("/tmp/screen_capture_status", "r");

		if(file != NULL){
			screen_rec_status = "REC |";
			fclose(file);
		}else{
			screen_rec_status = "";
		}

		used_memory = execscript("free -h | awk '(NR==2){print $3}'");
		available_memory = execscript("free -h | awk '(NR==2){print $2}'");

		status = smprintf("%s Mic: %s | V: %s | Mem: %s/%s | %s" ,
				screen_rec_status , mic_status, volume, used_memory, available_memory,  time_date);
		setstatus(status);

		free(time_date);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}

