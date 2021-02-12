#include <pebble.h>
#include "face1.h"

//declare functions
int main(void);
static void init();
static void deinit();
static void main_window_load(Window* window);
static void main_window_unload(Window* window);
static void tick_handler(struct tm* tick_time, TimeUnits units_changed);
static void update_time();
static void inbox_received_callback(DictionaryIterator* iterator, void* context);
static void inbox_dropped_callback(AppMessageResult reason, void* context);
static void outbox_failed_callback(DictionaryIterator* iterator, AppMessageResult reason, void* context);
static void	outbox_sent_callback(DictionaryIterator* iterator, void* context);

//declare variables
static Window* s_main_window;
static TextLayer* s_time_layer;
static TextLayer* s_weather_layer;

int main(void){
	init();
	app_event_loop();
	deinit();
}

static void init(){
	//Create main Window element and assign to pointer
	s_main_window = window_create();

	//Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers){
		.load	= main_window_load,
		.unload	= main_window_unload
	});

	//Show the Window on the watch, with animated = true
	window_stack_push(s_main_window, true);

	//Make sure the time is displayed from the start
	update_time();

	//Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	//Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	//Open AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit(){
	//Destroy Window
	window_destroy(s_main_window);
}

static void main_window_load(Window* window){
	//Get information about the window
	Layer* window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	//Create the TextLayer with specific bounds
	s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

	//Create temperature Layer
	s_weather_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(125, 120), bounds.size.w, 25));	

	//Improve the layout to be more like a watchface
	//s_time_layer
	text_layer_set_background_color(s_time_layer, GColorDarkGreen);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_text(s_time_layer, "00:00");
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	//s_weather_layer
	text_layer_set_background_color(s_weather_layer, GColorDarkGreen);
	text_layer_set_text_color(s_weather_layer, GColorWhite);
	text_layer_set_text(s_weather_layer, "Loading...");
	text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);

	//Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
}

static void main_window_unload(Window* window){
	//Destroy TextLayer
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_weather_layer);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed){
	update_time();
}

static void update_time(){
	//Get a tm structure
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	//Write the current hours and minutes into a buffer
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style()? "%H:%M": "%I:%M", tick_time);

	//Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
}

static void inbox_received_callback(DictionaryIterator* iterator, void* context){

}

static void inbox_dropped_callback(AppMessageResult reason, void* context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator* iterator, AppMessageResult reason, void* context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void	outbox_sent_callback(DictionaryIterator* iterator, void* context){
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send success!");
}
