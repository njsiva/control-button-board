/*
 * Copyright (C) 2024 J Siva
 *
 * This LV2 plugin is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This LV2 plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this LV2 plugin; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include "lv2-hmi.h"
#include "control-input-port-change-request.h"  // Include the extension header
#include <lv2/core/lv2.h>
#include <lv2/core/lv2_util.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/log/log.h>
#include <lv2/log/logger.h>
#include <lv2/midi/midi.h>
#include <lv2/urid/urid.h>



#define PLUGIN_URI "http://example.org/control-button-board"

// Number of toggle pairs
#define NUM_PAIRS 5

// Define the total number of ports
#define TOTAL_PORTS (NUM_PAIRS * 3+1)

// The main plugin struct to hold the toggle statuses and other state data
typedef struct {
	// Ports
	const float* toggles[NUM_PAIRS][2];
	float* cv_output[NUM_PAIRS];
	const LV2_Atom_Sequence* midi_in_port;
	// Features
	LV2_URID_Map*  map;
	LV2_HMI_WidgetControl* hmi;
	const LV2_ControlInputPort_Change_Request* change_request;  // Feature for control input port change request
	LV2_Log_Logger logger;

	// State variables
	uint8_t states[NUM_PAIRS];

	// HMI Widgets addressing
	LV2_HMI_Addressing toggle_addressing[NUM_PAIRS][2];
} TogglePlugin;

// Instantiate the plugin
static LV2_Handle instantiate(const LV2_Descriptor* descriptor,
							  double rate,
							  const char* bundle_path,
							  const LV2_Feature* const* features) {
	TogglePlugin* plugin = (TogglePlugin*)malloc(sizeof(TogglePlugin));

	// Query host features
	const char* missing = lv2_features_query(
			features,
			LV2_URID__map, &plugin->map, true,
			LV2_LOG__log, &plugin->logger.log,  false,
			LV2_HMI__WidgetControl, &plugin->hmi, false,
			LV2_CONTROL_INPUT_PORT_CHANGE_REQUEST_URI, &plugin->change_request, false,
			NULL);

	lv2_log_logger_set_map(&plugin->logger, plugin->map);
	if (missing) {
		lv2_log_error(&plugin->logger, "Missing feature <%s>\n", missing);
		free(plugin);
		return NULL;
	}
	for (int i = 0; i < NUM_PAIRS; i++) {
		plugin->states[i] = 0;
		plugin->toggle_addressing[i][0] = NULL;
		plugin->toggle_addressing[i][1] = NULL;
	}
	return (LV2_Handle)plugin;
}

// Connect host's data locations to the plugin's ports
static void connect_port(LV2_Handle instance,
						 uint32_t port,
						 void* data_location) {
	TogglePlugin* plugin = (TogglePlugin*)instance;
	if (port < 2*NUM_PAIRS) {
		uint8_t pair_index = port / 2;
		uint8_t toggle_index = port % 2;
		plugin->toggles[pair_index][toggle_index] = (const float*)data_location;
	} else if (port < TOTAL_PORTS-1) {
		uint8_t pair_index = port - 2*NUM_PAIRS;
		plugin->cv_output[pair_index] = (float*)data_location;
	} else if (port == TOTAL_PORTS-1) {
		plugin->midi_in_port = (const LV2_Atom_Sequence*)data_location;
	}
}

// Trigger LED change on the HMI based on toggle state
void trigger_led_change(TogglePlugin* plugin, uint8_t pair_index, uint8_t toggle_index, uint8_t onoff)
{
	// If we are turning on the widget, use the Red colour, if not, turn it off
	LV2_HMI_LED_Colour colour = onoff ? LV2_HMI_LED_Colour_Red : LV2_HMI_LED_Colour_Off;
	if (plugin->hmi && plugin->toggle_addressing[pair_index][toggle_index]) {
		// Send to HMI
		plugin->hmi->set_led_with_brightness(plugin->hmi->handle, plugin->toggle_addressing[pair_index][toggle_index], colour, LV2_HMI_LED_Brightness_High);
	}
}

// Main processing function
static void run(LV2_Handle instance, uint32_t n_samples) {
	TogglePlugin* plugin = (TogglePlugin*)instance;
	uint8_t midi_toggles[NUM_PAIRS];


	memset(midi_toggles,0,NUM_PAIRS*sizeof(uint8_t));

	// Read incoming events
	LV2_ATOM_SEQUENCE_FOREACH (plugin->midi_in_port, ev) {
		const uint8_t* const msg = (const uint8_t*)(ev + 1);
		switch (lv2_midi_message_type(msg)) {
		case LV2_MIDI_MSG_CONTROLLER:
			if( msg[1] == 0x13) {
				if (msg[2] < NUM_PAIRS ) {
					midi_toggles[msg[2]] = 1;
				}
			
			}
			
      default:
        // Forward all other MIDI events directly
        // lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
		#warning TODO: add output port
        break;
      }
	}
	for (int i = 0; i < NUM_PAIRS; i++) {
		// Check the toggle states
		uint8_t toggle1on = (*(plugin->toggles[i][0]) > 0.0f) ? 1 : 0;
		uint8_t toggle2on = (*(plugin->toggles[i][1]) > 0.0f) ? 1 : 0;

		uint8_t new_state = plugin->states[i];
#if 0
		// If the state has changed, update the state, CV output, and request the other toggle to change
		if (plugin->states[i] != toggle1on) {
			// this means something has happened with toggle1
			new_state = toggle1on;
		} else if (plugin->states[i] != toggle2on) {
			// this means something has happened with toggle1
			new_state = toggle2on;
		} 
#else
		if ( (new_state != toggle1on) || (new_state != toggle2on) || (midi_toggles[i]) )
			new_state = (new_state ? 0 : 1);
#endif	


		if(new_state != plugin->states[i]) {
			// Set the CV output to 10V if the state is on, 0V if off
			float cv_value = new_state ? 10.0f : 0.0f;

			for (uint32_t j = 0; j < n_samples; j++) {
				plugin->cv_output[i][j] = cv_value;
			}

			// Update the LED status based on the new state
			lv2_log_trace(&plugin->logger, "trigger LED change <%i> - 0 with <%u>\n", i, new_state);
			trigger_led_change(plugin, i, 0, new_state);
			lv2_log_trace(&plugin->logger, "trigger LED change <%i> - 1 with <%u>\n", i, new_state);
			trigger_led_change(plugin, i, 1, new_state);

			// Request the other toggle to change state if needed
			if (plugin->change_request) {
				// if the new state is on
				if (new_state) {
					//  try to turn what is not on on
					if (!toggle2on) {
						lv2_log_trace(&plugin->logger, "trying to change on switch for port <%i> with 1.0\n", ((i*2)+1));
						plugin->change_request->request_change(plugin->change_request->handle, (i * 2) + 1, 1.0);
					}
					if (!toggle1on) {
						lv2_log_trace(&plugin->logger, "trying to change on switch for port <%i> with 1.0\n", (i*2));
						plugin->change_request->request_change(plugin->change_request->handle, (i * 2), 1.0);
					}
				} else {
					//  try to turn what is not off off
					if (toggle1on) {
						lv2_log_trace(&plugin->logger, "trying to change on switch for port <%i> with 0.0\n", (i*2));
						plugin->change_request->request_change(plugin->change_request->handle, (i * 2), 0.0);
					}
					if (toggle2on) {
						lv2_log_trace(&plugin->logger, "trying to change on switch for port <%i> with 0.0\n", ((i*2)+1));
						plugin->change_request->request_change(plugin->change_request->handle, (i * 2) + 1, 0.0);
					}
				}
			}
			plugin->states[i] = new_state;
		}
	}
}


// Cleanup the plugin instance
static void cleanup(LV2_Handle instance) {
	free(instance);
}

// Handle addressing of HMI widgets
static void addressed(LV2_Handle handle, uint32_t index, LV2_HMI_Addressing addressing, const LV2_HMI_AddressingInfo* info)
{
	TogglePlugin* plugin = (TogglePlugin*) handle;
	uint8_t pair_index = index / 2;
	uint8_t toggle_index = index % 2;

	plugin->toggle_addressing[pair_index][toggle_index] = addressing;
}

// Handle unaddressing of HMI widgets
static void unaddressed(LV2_Handle handle, uint32_t index)
{
	TogglePlugin* plugin = (TogglePlugin*) handle;
	uint8_t pair_index = index / 2;
	uint8_t toggle_index = index % 2;

	plugin->toggle_addressing[pair_index][toggle_index] = NULL;
}

// Extension data for the HMI feature
static const void* extension_data(const char* uri)
{
	static const LV2_HMI_PluginNotification hmiNotif = {
		addressed,
		unaddressed,
	};

	if (!strcmp(uri, LV2_HMI__PluginNotification))
		return &hmiNotif;

	return NULL;
}

// Descriptor for the plugin
static const LV2_Descriptor descriptor = {
	PLUGIN_URI,
	instantiate,
	connect_port,
	NULL,  // Activate function not used
	run,
	NULL,  // Deactivate function not used
	cleanup,
	extension_data
};

// Entry point for the plugin
LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index) {
	return (index == 0) ? &descriptor : NULL;
}

