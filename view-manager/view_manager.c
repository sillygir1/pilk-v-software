#include "view_manager.h"

ViewManager *view_manager_init(uint16_t view_count) {
  ViewManager *view_manager = calloc(1, sizeof(*view_manager));
  view_manager->view_count = view_count + 1;
  view_manager->view = calloc(1, sizeof(*view_manager->view) * view_count);
  for (uint16_t i = 0; i < view_count; i++) {
    view_manager->view[i] = NULL;
  }
  static lv_style_t parent_style;
  lv_style_init(&parent_style);
  lv_style_set_width(&parent_style, 240);
  lv_style_set_height(&parent_style, 295);
  lv_style_set_y(&parent_style, 25);
  lv_style_set_radius(&parent_style, 0);
  lv_style_set_pad_all(&parent_style, 0);
  view_manager->screen_main = lv_scr_act();
  view_manager->obj_parent = lv_obj_create(view_manager->screen_main);
  lv_obj_add_style(view_manager->obj_parent, &parent_style, LV_PART_MAIN);
  view_manager->current_view = 0;
  return view_manager;
}

void view_manager_add_view(ViewManager *view_manager,
                           void (*init)(void *, void *), void (*exit)(),
                           uint16_t number) {
  printf("Adding view\n");
  if (view_manager->view[number]) {
    printf("View %u already exists!\n", number);
  }
  view_manager->view[number] = calloc(1, sizeof(*view_manager->view[number]));
  view_manager->view[number]->init = init;
  view_manager->view[number]->exit = exit;
  view_manager->view_count += 1;
  printf("Adding view end\n");
}

int view_manager_switch_view(ViewManager *view_manager, uint16_t number,
                             void *ctx) {
  printf("Switching view\n");
  if (!view_manager->view[number]) {
    printf("View %u is not initialized", number);
    return 1;
  }
  if (view_manager->current_view != 0)
    view_manager->view[view_manager->current_view]->exit();
  view_manager->current_view = number;
  view_manager->view[view_manager->current_view]->init(view_manager, ctx);
  return 0;
}

void view_manager_remove_view(ViewManager *view_manager, uint16_t number) {
  if (view_manager->view[number]) {
    free(view_manager->view[number]);
    view_manager->view[number] = NULL;
  }
  view_manager->view_count -= 1;
}

void view_manager_free(ViewManager *view_manager) {
  if (!view_manager)
    return;
  uint16_t view_cnt = view_manager->view_count;
  for (uint16_t i = 0; i < view_cnt; i++) {
    view_manager_remove_view(view_manager, i);
  }
  free(view_manager->view);
  lv_obj_del(view_manager->screen_main);
  free(view_manager);
}
