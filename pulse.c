#include "pulse.h"
#include <complex.h>
#include <pulse/def.h>
#include <pulse/operation.h>
#include <pulse/volume.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>


pa_context* pulse_context;
pa_mainloop* pulse_main_loop;
pa_mainloop_api* pulse_mainloop_api;	

uint8_t volume_percentage = 0;

pthread_t pulse_thread;

static void pulse_sink_info_callback(pa_context* context,
		const pa_sink_info* information,
		int eol, void* userdata){
	
	printf("sink info\n");

	if(information){

		float volume = (float)pa_cvolume_avg(&(information->volume)) / 
				(float)PA_VOLUME_NORM;
		float percentage = volume * 100.0f;
		volume_percentage = (uint8_t)percentage;

	}

}

static void pulse_server_info_callback(pa_context* context,
		const pa_server_info* information,
		void* userdata){

	printf("pulse server info\n");
	printf("%s\n", information->default_sink_name);
	pa_context_get_sink_info_by_name(context,
						information->default_sink_name,
						pulse_sink_info_callback, userdata);
	


}

static void pulse_suscribe_callback(pa_context* context,
		pa_subscription_event_type_t type, uint32_t index,
		void* userdata){
	
	unsigned facility = type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
	
	pa_operation* operation = NULL;

	switch(facility){
		case PA_SUBSCRIPTION_EVENT_SINK:
			operation = pa_context_get_sink_info_by_index(context,
					index, pulse_sink_info_callback, NULL);
			break;
		default:
			break;
	}

	if(operation)
		pa_operation_unref(operation);
}

static void pulse_context_state_callback(pa_context* context, void * userdata){
	switch (pa_context_get_state(context)) {
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_TERMINATED:
		case PA_CONTEXT_READY:
			printf("Connection stablished\n");
			pa_context_get_server_info(context,
					pulse_server_info_callback,
					NULL);
			pa_context_set_subscribe_callback(context,
					pulse_suscribe_callback,
					NULL);
			pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK,
					NULL, NULL);
			break;
		default:
			printf("ERROR connecting pulse server\n");
	}
}

void* pulse_run(void*data){
		pulse_main_loop =  pa_mainloop_new();
		pulse_mainloop_api = pa_mainloop_get_api(pulse_main_loop);

		pulse_context = pa_context_new(pulse_mainloop_api, "Dwstatus Pulse");
	
		pa_context_connect(pulse_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);
	
		pa_context_set_state_callback(pulse_context, pulse_context_state_callback,
			NULL);

		printf("pulse init\n");
		int ret = 0;
		pa_mainloop_run(pulse_main_loop, &ret);

}

void pulse_init(){
	pthread_create(&pulse_thread, NULL, pulse_run, NULL);
}
