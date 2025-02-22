#include "apps_screen_view.h"
#include "view_list.h"

static lv_obj_t *list;
static ViewManager *view_manager;

#define APP_NUM 6
static const char *app_names[APP_NUM] = {"Back", "File manager", "app2",
                                         "app3", "app4",         "app5"};
static const char *app_cmds[APP_NUM] = {"", "file-manager", "", "", "", ""};
static const char *app_icons[APP_NUM] = {
    LV_SYMBOL_BACKSPACE, LV_SYMBOL_DIRECTORY, LV_SYMBOL_LIST,
    LV_SYMBOL_LIST,      LV_SYMBOL_LIST,      LV_SYMBOL_LIST};

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // Handle click
    if (strcmp(button_text, "Back") == 0) {
      view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
      return;
    }
    for (int i = 1; i < APP_NUM; i++) {
      if (strcmp(button_text, app_names[i]) == 0) {
        apps_exit();
        launch_app(app_cmds[i]);
        apps_init(view_manager, NULL);
      }
    }
  } else if (code == LV_EVENT_KEY &&
             lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
    // printf("Back button pressed!\n");
    view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
  }
}

void apps_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  set_mode_text("Applications");

  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  ViewManagerList vm_list = {
      app_names, (const void **)app_icons, APP_NUM, NULL, 0, event_handler};
  list = view_manager_list_init(view_manager, &vm_list);
}

void apps_exit() { lv_obj_del(list); }
