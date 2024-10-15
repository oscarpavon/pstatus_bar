#include "pulse.h"
#include <pulse/volume.h>

static void pulse_sink_info_callback(pa_context* context,
		const pa_sink_info* information,
		int eol, void* userdata){

	if(information){
		float volume = (float)pa_cvolume_avg(&(information->volume)) / 
				(float)PA_VOLUME_NORM;
		
		volume_percentage = volume * 100.0f;
	}

}

static void pulse_server_info_callback(pa_context* context,
		const pa_server_info* information,
		void* userdata){

				pa_context_get_sink_info_by_name(context,
						information->default_sink_name,
						pulse_sink_info_callback, userdata);

}

static void context_state_callback(pa_context* context, void * userdata){
	switch (pa_context_get_state(context)) {
		case PA_CONTEXT_READY:
			pa_context_get_server_info(context,
					pulse_server_info_callback,
					userdata);
	}
}

void pulse_init(){

	pulse_main_loop =  pa_mainloop_new();
	pulse_mainloop_api = pa_mainloop_get_api(pulse_main_loop);

	pulse_context = pa_context_new(pulse_mainloop_api, "Dwstatus Pulse");
	
	pa_context_connect(pulse_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);
	
	pa_context_set_state_callback(pulse_context, context_state_callback, NULL);

}
