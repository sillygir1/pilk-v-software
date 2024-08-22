#include "apps_screen.h"
#include "main_screen.h"

static lv_obj_t *apps_menu_list;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(apps_menu_list, obj);
    // Handle click
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      // Do nothing on the main screen
      printf("Back button pressed!\n");
      lv_obj_del(apps_menu_list);
      menu_ui();
    }
  }
}

void apps_ui() {

  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_width(&style, 240);
  lv_style_set_height(&style, 295);
  lv_style_set_y(&style, 25);

  apps_menu_list = lv_list_create(lv_scr_act());
  lv_obj_add_style(apps_menu_list, &style, LV_PART_MAIN);

  lv_obj_t *btn;

  // Dummy items
  lv_list_add_text(apps_menu_list, "Category 1");
  btn = lv_list_add_btn(apps_menu_list, LV_SYMBOL_DRIVE, "App1");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(apps_menu_list, LV_SYMBOL_DRIVE, "App2");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  lv_list_add_text(apps_menu_list, "Category 2");
  btn = lv_list_add_btn(apps_menu_list, LV_SYMBOL_DRIVE, "App3");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(apps_menu_list, LV_SYMBOL_DRIVE, "App4");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
}