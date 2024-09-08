#include "apps_screen_view.h"
#include "view_list.h"

static lv_obj_t *list;
static ViewManager *view_manager;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // Handle click
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      // Do nothing on the main screen
      printf("Back button pressed!\n");
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
    }
  }
}

void apps_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  set_mode_text("Applications");

  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);

  lv_obj_t *btn;

  // Dummy items
  lv_list_add_text(list, "Category 1");
  btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, "App1");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, "App2");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  lv_list_add_text(list, "Category 2");
  btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, "App3");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  btn = lv_list_add_btn(list, LV_SYMBOL_DRIVE, "App4");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
}

void apps_exit() { lv_obj_del(list); }