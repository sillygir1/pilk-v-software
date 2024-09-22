#include "file_manager.h"

#define DIR_LIST_LEN 128
#ifndef FM_PLUGIN
#define BACK_BUTTON "Exit"
#else
#define BACK_BUTTON "Back"
#endif

static lv_obj_t *list;
static ViewManager *view_manager;
static FileManagerData *fm_data;
static bool skip_dirs;

void file_manager_update_list() {
  printf("Updating list\n");
  char *arr[DIR_LIST_LEN];
  set_mode_text(fm_data->dir);
  lv_obj_clean(list);
  lv_obj_t *btn;

  btn = lv_list_add_btn(list, LV_SYMBOL_BACKSPACE, BACK_BUTTON);
  lv_obj_add_event_cb(btn, fm_data->event_handler, LV_EVENT_ALL, view_manager);

  int n = storage_dir_list(fm_data->dir, arr, DIR_LIST_LEN, skip_dirs);
  if (n == 0) {
    btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "No saved files");
    lv_obj_add_event_cb(btn, fm_data->event_handler, LV_EVENT_KEY,
                        view_manager);
  } else {
    for (uint8_t i = 0; i < n; i++) {
      if (!storage_is_dir(fm_data->dir, arr[i]))
        continue;
      btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, arr[i]);
      lv_obj_add_event_cb(btn, fm_data->event_handler, LV_EVENT_ALL,
                          view_manager);
    }
    for (uint8_t i = 0; i < n; i++) {
      if (storage_is_dir(fm_data->dir, arr[i]))
        continue;
      btn = lv_list_add_btn(list, LV_SYMBOL_FILE, arr[i]);
      lv_obj_add_event_cb(btn, fm_data->event_handler, LV_EVENT_ALL,
                          view_manager);
    }
    for (uint8_t i = 0; i < n; i++) {
      free(arr[i]);
    }
  }
}

void file_manager_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);

  if (!fm_data)
    fm_data = ctx;
  fm_data->leaving = false;

  skip_dirs = true;
  if (fm_data->file_type == 0) {
    skip_dirs = false;
  }

  file_manager_update_list();
}

void file_manager_exit() {
  if (fm_data && fm_data->leaving) {
    free(fm_data);
    fm_data = NULL;
  }
  lv_obj_del(list);
}

lv_obj_t *file_manager_glue_stick() { return list; }