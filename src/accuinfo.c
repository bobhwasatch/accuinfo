/****************************************************************************/
/**
* Pebble watch face that displays "all the time".
*
* @file   accuinfo.c
*
* @author Bob Hauck <bobh@haucks.org>
*
* This code may be used or modified in any way whatsoever without permission
* from the author.
*
*****************************************************************************/
#include <ctype.h>
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "style.h"


#if (INVERTED == 1)
#define MY_UUID {0x50, 0x6B, 0xB2, 0xE6, 0x43, 0x82, 0x47, 0x0D,        \
                 0xBB, 0x4B, 0x8A, 0xEA, 0x22, 0xBB, 0xE2, 0x03 }
#define APP_NAME "AccuWhite"
#define FG_COLOR GColorBlack
#define BG_COLOR GColorWhite

#else

#define MY_UUID { 0x85, 0x98, 0xEA, 0x3A, 0x13, 0x8C, 0x4F, 0x9D,       \
                  0x89, 0x17, 0xF3, 0x84, 0xD6, 0x95, 0xD7, 0x6B }
#define APP_NAME "AccuInfo"
#define FG_COLOR GColorWhite
#define BG_COLOR GColorBlack
#endif

PBL_APP_INFO(MY_UUID,
             APP_NAME,
             "bobh@haucks.org",
             1, 1, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);


Window window;
TextLayer day_layer;
TextLayer date_layer;
TextLayer time_layer;
TextLayer secs_layer;
TextLayer ampm_layer;
TextLayer year_layer;
Layer line_layer;


char *upcase(char *str)
{
    char *s = str;

    while (*s)
    {
        *s++ = toupper((int)*s);
    }

    return str;
}


void line_layer_update_callback(Layer *l, GContext *ctx)
{
    (void)l;

    graphics_context_set_stroke_color(ctx, FG_COLOR);
    graphics_draw_line(ctx, GPoint(0, 35), GPoint(144, 35));
    graphics_draw_line(ctx, GPoint(0, 36), GPoint(144, 36));
    graphics_draw_line(ctx, GPoint(0, 108), GPoint(144, 108));
    graphics_draw_line(ctx, GPoint(0, 109), GPoint(144, 109));
}


void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    (void)ctx;

    // Need to be static because they're used by the system later.
    static char year_text[] = "0000";
    static char date_text[] = "Xxxxxxxxx 00";
    static char day_text[] = "Xxxxxxxxx";
    static char time_text[] = "00:00";
    static char secs_text[] = "00";
    static char ampm_text[] = "XX";

    if (t->units_changed & DAY_UNIT)
    {
        string_format_time(day_text, sizeof(day_text), "%A", t->tick_time);
        text_layer_set_text(&day_layer, upcase(day_text));

        string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
        text_layer_set_text(&date_layer, date_text);

        string_format_time(year_text, sizeof(year_text), "%Y", t->tick_time);
        text_layer_set_text(&year_layer, year_text);
    }

    if (clock_is_24h_style())
    {
        strcpy(ampm_text, "  ");
        string_format_time(time_text, sizeof(time_text), "%R", t->tick_time);
    }
    else
    {
        string_format_time(ampm_text, sizeof(ampm_text), "%p", t->tick_time);
        string_format_time(time_text, sizeof(time_text), "%I:%M", t->tick_time);

        // Kludge to handle lack of non-padded hour format string
        // for twelve hour clock.
        if (!clock_is_24h_style() && (time_text[0] == '0'))
        {
            memmove(time_text, &time_text[1], sizeof(time_text) - 1);
        }
    }

    string_format_time(secs_text, sizeof(secs_text), "%S", t->tick_time);
    text_layer_set_text(&time_layer, time_text);
    text_layer_set_text(&secs_layer, secs_text);
    text_layer_set_text(&ampm_layer, ampm_text);
}


void handle_init(AppContextRef ctx)
{
    PblTm tm;
    PebbleTickEvent t;
    ResHandle res_d;
    ResHandle res_t;
    GFont font_date;
    GFont font_time;

    window_init(&window, "AccuInfo");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, BG_COLOR);

    resource_init_current_app(&APP_RESOURCES);

    res_d = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21);
    res_t = resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_41);
    font_date = fonts_load_custom_font(res_d);
    font_time = fonts_load_custom_font(res_t);

    text_layer_init(&day_layer, window.layer.frame);
    text_layer_set_text_color(&day_layer, FG_COLOR);
    text_layer_set_background_color(&day_layer, GColorClear);
    text_layer_set_text_alignment(&day_layer, GTextAlignmentCenter);
    text_layer_set_font(&day_layer, font_date);
    layer_set_frame(&day_layer.layer, GRect(1, 2, 144-1, 168-2));
    layer_add_child(&window.layer, &day_layer.layer);

    text_layer_init(&time_layer, window.layer.frame);
    text_layer_set_text_color(&time_layer, FG_COLOR);
    text_layer_set_background_color(&time_layer, GColorClear);
    layer_set_frame(&time_layer.layer, GRect(1, 48, 144-1, 168-48));
    text_layer_set_font(&time_layer, font_time);
    layer_add_child(&window.layer, &time_layer.layer);

    text_layer_init(&secs_layer, window.layer.frame);
    text_layer_set_text_color(&secs_layer, FG_COLOR);
    text_layer_set_background_color(&secs_layer, GColorClear);
    layer_set_frame(&secs_layer.layer, GRect(112, 46, 144-112, 168-46));
    text_layer_set_font(&secs_layer, font_date);
    layer_add_child(&window.layer, &secs_layer.layer);

    text_layer_init(&ampm_layer, window.layer.frame);
    text_layer_set_text_color(&ampm_layer, FG_COLOR);
    text_layer_set_background_color(&ampm_layer, GColorClear);
    layer_set_frame(&ampm_layer.layer, GRect(112, 74, 144-112, 168-74));
    text_layer_set_font(&ampm_layer, font_date);
    layer_add_child(&window.layer, &ampm_layer.layer);

    text_layer_init(&date_layer, window.layer.frame);
    text_layer_set_text_color(&date_layer, FG_COLOR);
    text_layer_set_background_color(&date_layer, GColorClear);
    text_layer_set_font(&date_layer, font_date);
    text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
    layer_set_frame(&date_layer.layer, GRect(1, 118, 144-1, 168-118));
    layer_add_child(&window.layer, &date_layer.layer);

    text_layer_init(&year_layer, window.layer.frame);
    text_layer_set_text_color(&year_layer, FG_COLOR);
    text_layer_set_background_color(&year_layer, GColorClear);
    text_layer_set_font(&year_layer, font_date);
    text_layer_set_text_alignment(&year_layer, GTextAlignmentCenter);
    layer_set_frame(&year_layer.layer, GRect(0, 142, 144, 168-142));
    layer_add_child(&window.layer, &year_layer.layer);

    layer_init(&line_layer, window.layer.frame);
    line_layer.update_proc = &line_layer_update_callback;
    layer_add_child(&window.layer, &line_layer);

    get_time(&tm);
    t.tick_time = &tm;
    t.units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;

    handle_second_tick(ctx, &t);
}


void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
        .init_handler = &handle_init,
        .tick_info =
        {
            .tick_handler = &handle_second_tick,
            .tick_units = SECOND_UNIT
        }
    };

    app_event_loop(params, &handlers);
}
