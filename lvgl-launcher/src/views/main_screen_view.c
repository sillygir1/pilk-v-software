#include "main_screen_view.h"
#include "view_list.h"

static lv_obj_t *list;
static ViewManager *view_manager;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    if (strcmp(button_text, "Proxmark3 client") == 0) {
      launch_client();
      menu_init(view_manager, NULL);
    } else if (strcmp(button_text, "Apps") == 0) {
      view_manager_switch_view(view_manager, VIEW_APPS, NULL);
    } else if (strcmp(button_text, "UART config") == 0) {
      printf("Opening uart config...\n");
      // Do the thing
    } else if (strcmp(button_text, "USB config") == 0) {
      printf("Opening usb config...\n");
      // Do the thing
    } else if (strcmp(button_text, "System settings") == 0) {
      printf("Opening system settings...\n");
      // Do the thing
    } else if (strcmp(button_text, "Reboot") == 0) {
      lv_obj_clean(lv_scr_act());
      system("reboot");
    } else if (strcmp(button_text, "Power off") == 0) {
      lv_obj_clean(lv_scr_act());
      system("poweroff");
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      // Do nothing on the main screen
      // printf("Back button pressed!\n");
    }
  }
}

void menu_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  set_mode_text("Pilk-V Duo");

  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);

  lv_obj_t *btn;

  btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, "Proxmark3 client");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, "Apps");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);

  lv_list_add_text(list, "Settings");
  btn = lv_list_add_btn(list, LV_SYMBOL_SHUFFLE, "UART config");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(list, LV_SYMBOL_USB, "USB config");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "System settings");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);

  lv_list_add_text(list, "Power");
  btn = lv_list_add_btn(list, LV_SYMBOL_REFRESH, "Reboot");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(list, LV_SYMBOL_POWER, "Power off");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
}

void menu_exit() {
  if (list)
    lv_obj_del(list);
}