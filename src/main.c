#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_text_layer;
static TextLayer *s_time_layer;
static BitmapLayer *s_bitmap_layer;
static BitmapLayer *s_line_layer;
const int TEXT_LAYER_HEIGHT = 15;
const int TIME_LAYER_HEIGHT = 45;
const int LINE_LAYER_HEIGHT = 2;
const int BITMAP_LAYER_HEIGHT = 116;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  int hour = 23 - tick_time->tm_hour;
  int min = 60 - tick_time->tm_min;
  snprintf(buffer, 6, "%.2d:%.2d", hour, min);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  // Update BitmapLayer
  int rest = 1440 - ((hour * 60) + min);
  int height = (rest * BITMAP_LAYER_HEIGHT) / 1440;
  layer_set_frame(bitmap_layer_get_layer(s_bitmap_layer), GRect(0, TIME_LAYER_HEIGHT + TEXT_LAYER_HEIGHT + LINE_LAYER_HEIGHT + (BITMAP_LAYER_HEIGHT - height) + 1, 144, height));
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_text_layer = text_layer_create(GRect(0, 0, 144, TEXT_LAYER_HEIGHT));
  s_time_layer = text_layer_create(GRect(0, TEXT_LAYER_HEIGHT + 1, 144, TIME_LAYER_HEIGHT));
  s_line_layer = bitmap_layer_create(GRect(0, TIME_LAYER_HEIGHT + TEXT_LAYER_HEIGHT + 1, 144, LINE_LAYER_HEIGHT));
  s_bitmap_layer = bitmap_layer_create(GRect(0, TEXT_LAYER_HEIGHT + TIME_LAYER_HEIGHT + LINE_LAYER_HEIGHT + 1, 144, BITMAP_LAYER_HEIGHT));
  bitmap_layer_set_background_color(s_bitmap_layer, GColorBlack);
  bitmap_layer_set_background_color(s_line_layer, GColorBlack);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_text_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_text_layer, "Rest of Your Time");

  // Improve the layout to be more like a watchface 
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD ));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bitmap_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_line_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
