#ifndef __PULSE_H__
#define __PULSE_H__
#include <pulse/pulseaudio.h>
#include <pthread.h>

extern pa_context* pulse_context;
extern pa_mainloop* pulse_main_loop;
extern pa_mainloop_api* pulse_mainloop_api;	

extern uint8_t volume_percentage;

void pulse_init();

#endif
