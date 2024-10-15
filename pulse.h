#ifndef __PULSE_H__
#define __PULSE_H__
#include <pulse/pulseaudio.h>

pa_context* pulse_context;
pa_mainloop* pulse_main_loop;
pa_mainloop_api* pulse_mainloop_api;	

float volume_percentage;

void pulse_init();

#endif
