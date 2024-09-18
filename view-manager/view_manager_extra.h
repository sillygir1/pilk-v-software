#include "view_manager.h"

/// @brief Initialize basic list view
/// @param view_manager view manager struct
/// @param elements list elements
/// @param icons list icons
/// @param element_cnt number of list elements
/// @param labels numbers of text elements
/// @param labels_cnt length of numbers array
/// @param event_handler input event handler
/// @return pointer to lvgl list
lv_obj_t *view_manager_list_init(ViewManager *view_manager,
                                 ViewManagerList *vm_list);
