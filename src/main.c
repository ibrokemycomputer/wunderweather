#include <pebble.h>
#define KEY_TEMP_F 0
#define KEY_TEMP_C 1
#define KEY_USE_C 2
#define KEY_ICON 3
#define KEY_TWENTY_FOUR_HOUR_FORMAT 4
#define KEY_COLOR 5

static Layer *s_battery_layer, *s_icon_layer;
static int s_battery_level, s_icon;
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_weather_layer;
static GFont s_time_font, s_date_font, s_weather_font;
static GDrawCommandImage *s_command_image;
static bool twenty_four_hour_format = false;
static bool use_c = 0;
static bool color = 0;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[] = "0000";
  if (clock_is_24h_style() == twenty_four_hour_format) {
    strftime(s_buffer, sizeof("0000"), "%I%M", tick_time);
    } else {
    strftime(s_buffer, sizeof("0000"), "%H%M", tick_time);
    }
  text_layer_set_text(s_time_layer, s_buffer);
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d", tick_time);
  text_layer_set_text(s_date_layer, date_buffer);
}

static void draw_black() {
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  window_set_background_color(s_main_window, GColorBlack);
  gdraw_command_image_destroy(s_command_image);
  if (s_icon == 0) {
    gdraw_command_image_destroy(s_command_image);
    text_layer_set_text(s_weather_layer, "");
  } else if (s_icon == 1) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLEAR);
  }
  else if (s_icon == 2) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLOUDY);
  }
  else if (s_icon == 3) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_FOG);
  }
  else if (s_icon == 4) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_LIGHT_RAIN);
  }
  else if (s_icon == 5) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_PARTLY_CLOUDY);
  }
  else if (s_icon == 6) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_RAIN);
  }
  else if (s_icon == 7) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_SNOW);
  }
  else if (s_icon == 8) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_THUNDER);
  }
  else if (s_icon >= 9) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_UNKNOWN);
  } 
}

static void draw_white() {
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  window_set_background_color(s_main_window, GColorWhite);
  gdraw_command_image_destroy(s_command_image);
  if (s_icon == 0) {
    gdraw_command_image_destroy(s_command_image);
    text_layer_set_text(s_weather_layer, "");

  } else if (s_icon == 1) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLEAR_I);
  }
  else if (s_icon == 2) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLOUDY_I);
  }
  else if (s_icon == 3) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_FOG_I);
  }
  else if (s_icon == 4) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_LIGHT_RAIN_I);
  }
  else if (s_icon == 5) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_PARTLY_CLOUDY_I);
  }
  else if (s_icon == 6) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_RAIN_I);
  }
  else if (s_icon == 7) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_SNOW_I);
  }
  else if (s_icon == 8) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_THUNDER_I);
  }
  else if (s_icon >= 9) {
    s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_UNKNOWN_I);
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temp_f_buffer[8];
  static char temp_c_buffer[8];
  static char weather_layer_buffer[32];

  Tuple *temp_f_tuple = dict_find(iterator, KEY_TEMP_F);
  Tuple *temp_c_tuple = dict_find(iterator, KEY_TEMP_C);
  Tuple *use_c_t = dict_find(iterator, KEY_USE_C);
  Tuple *icon_t = dict_find(iterator, KEY_ICON);
  Tuple *twenty_four_hour_format_t = dict_find(iterator, KEY_TWENTY_FOUR_HOUR_FORMAT);
  Tuple *color_t = dict_find(iterator, KEY_COLOR);

  if (use_c_t) {
  	use_c = use_c_t->value->int8;
  	persist_write_int(KEY_USE_C, use_c);
  }
  if(temp_f_tuple) {
    snprintf(temp_f_buffer, sizeof(temp_f_buffer), "%u°", (int)temp_f_tuple->value->int32);
  }
  if(temp_c_tuple) {
    snprintf(temp_c_buffer, sizeof(temp_c_buffer), "%u°", (int)temp_c_tuple->value->int32);
  }
  if (use_c == 1) {
      snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s°", temp_f_buffer);
    	text_layer_set_text(s_weather_layer, temp_c_buffer);
    } else {
      snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s°", temp_c_buffer);
    	text_layer_set_text(s_weather_layer, temp_f_buffer);
  }
  if (color_t) {
  	color = color_t->value->int8;
  	persist_write_int(KEY_COLOR, color);
  }
  if (twenty_four_hour_format_t) {
    twenty_four_hour_format = twenty_four_hour_format_t->value->int8;
    persist_write_int(KEY_TWENTY_FOUR_HOUR_FORMAT, twenty_four_hour_format);
    update_time();
  }
  if (icon_t) {
    s_icon = icon_t->value->int32;
    persist_write_int(KEY_ICON, s_icon);
  }
  
  if (KEY_COLOR) {
    color = persist_read_int(KEY_COLOR);
    if (color != 1) {
      draw_black();
    } else if (color == 1) {
      draw_white();
    }
  } else {
    draw_black();
  }
}

static void battery_callback(BatteryChargeState state) {
      s_battery_level = state.charge_percent;
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int height = (int)(float)(168 - ((float)(((float)s_battery_level / 100.0F) * 168.0F)));
  if (color != 1) {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
    graphics_context_set_fill_color(ctx, GColorClear);
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, height), 0, GCornerNone);
  } else {
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, height), 0, GCornerNone);
  }
}

static void update_proc(Layer *layer, GContext *ctx) {
  GPoint origin = GPoint(0, 0);
  gdraw_command_image_draw(ctx, s_command_image, origin);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  if(tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
    battery_state_service_subscribe(battery_callback);
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_time_layer = text_layer_create(GRect(0, 40, bounds.size.w, 60));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONICAS_SALON_58));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  s_date_layer = text_layer_create(GRect(0, 100, bounds.size.w, 32));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONICAS_SALON_32));
  text_layer_set_font(s_date_layer, s_date_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_text(s_date_layer, "Xxx 00");
  s_weather_layer = text_layer_create(GRect(-30, 132, bounds.size.w, 32));
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentRight);
  text_layer_set_text(s_weather_layer, "Loading...");
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONICAS_SALON_32));
  text_layer_set_font(s_weather_layer, s_weather_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  s_icon_layer = layer_create(GRect(117, 140, bounds.size.w, bounds.size.h));
  layer_set_update_proc(s_icon_layer, update_proc);
  layer_add_child(window_layer, s_icon_layer);
  s_battery_layer = layer_create(GRect(0, 0, 8, bounds.size.h));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  layer_mark_dirty(s_battery_layer);
  if (persist_read_bool(KEY_TWENTY_FOUR_HOUR_FORMAT)) {
    twenty_four_hour_format = persist_read_bool(KEY_TWENTY_FOUR_HOUR_FORMAT);
  }
  if (persist_exists(KEY_USE_C)) {
  	use_c = persist_read_int(KEY_USE_C);
  }
  if (persist_exists(KEY_COLOR)) {
  	color = persist_read_int(KEY_COLOR);
  }
  if (persist_exists(KEY_ICON)) {
  	s_icon = persist_read_int(KEY_ICON);
  }
  if (color != 1) {
    draw_black();
  } else if (color == 1) {
    draw_white();
  } else {
    draw_black();
  }
  update_time();
}

static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_date_font);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weather_layer);
  fonts_unload_custom_font(s_weather_font);
  layer_destroy(s_battery_layer);
  layer_destroy(s_icon_layer);
  gdraw_command_image_destroy(s_command_image);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_callback(battery_state_service_peek());
  app_message_register_inbox_received(inbox_received_callback);
  const int inbox_size = 1024;
  const int outbox_size = 1024;
  app_message_open(inbox_size, outbox_size);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
