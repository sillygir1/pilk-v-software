#include "view_manager_extra.h"

lv_obj_t *view_manager_list_init(ViewManager *view_manager,
                                 ViewManagerList *vm_list) {
  lv_obj_t *list = lv_list_create(view_manager->obj_parent);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);
  lv_obj_set_width(list, 240);
  lv_obj_set_height(list, 295);

  lv_obj_t *btn;

  for (int i = 0; i < vm_list->element_cnt; i++) {
    bool skip = 0;
    if (vm_list->labels && vm_list->labels_cnt) {
      for (int j = 0; j < vm_list->labels_cnt; j++) {
        if (vm_list->labels[j] == i) {
          lv_list_add_text(list, vm_list->elements[i]);
          skip = 1;
          break;
        }
      }
    }
    if (skip)
      continue;
    btn = lv_list_add_btn(list, vm_list->icons[i], vm_list->elements[i]);
    lv_obj_add_event_cb(btn, vm_list->event_handler, LV_EVENT_ALL, NULL);
  }

  return list;
}
