#include "view_manager.h"

ViewManager *view_manager_init(uint16_t view_count) {
  ViewManager *view_manager = malloc(sizeof(*view_manager));
  view_manager->view_count = view_count;
  view_manager->view = malloc(sizeof(*view_manager->view) * view_count);
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
  view_manager->obj_parent = lv_obj_create(lv_scr_act());
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
  view_manager->view[number] = malloc(sizeof(*view_manager->view[number]));
  view_manager->view[number]->init = init;
  view_manager->view[number]->exit = exit;
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
}

void view_manager_free(ViewManager *view_manager) {
  if (!view_manager)
    return;
  for (uint16_t i = 0; i < view_manager->view_count; i++) {
    view_manager_remove_view(view_manager, i);
  }
  free(view_manager->view);
  free(view_manager);
}

// TODO: Move somewhere else later
lv_obj_t *view_manager_list_init(ViewManager *view_manager, char **elements,
                                 const void **icons, uint32_t element_cnt,
                                 int *labels, int labels_cnt,
                                 void (*event_handler)(lv_event_t *)) {
  lv_obj_t *list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);

  lv_obj_t *btn;

  for (int i = 0; i < element_cnt; i++) {
    bool skip = 0;
    if (labels && labels_cnt) {
      for (int j = 0; j < labels_cnt; j++) {
        if (labels[j] == i) {
          lv_list_add_text(list, elements[i]);
          skip = 1;
          break;
        }
      }
    }
    if (skip)
      continue;
    btn = lv_list_add_btn(list, icons[i], elements[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  }

  return list;
}