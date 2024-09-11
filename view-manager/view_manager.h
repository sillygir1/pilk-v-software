#pragma once

#include "encoder.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/lvgl.h"

typedef struct {
  void (*init)(void *, void *);
  void (*exit)();
} View;

typedef struct {
  View **view;
  uint16_t current_view;
  lv_obj_t *obj_parent;
  uint16_t view_count;
} ViewManager;

/// @brief Initialize view manager
/// @return Pointer to view manager struct
ViewManager *view_manager_init(uint16_t view_count);

/// @brief Add view to the view manager
/// @param view_manager view manager struct
/// @param init init function of the view
/// @param input input function of the view
/// @param exit exit function of the view
/// @param number view's unique number
void view_manager_add_view(ViewManager *view_manager,
                           void (*init)(void *, void *), void (*exit)(),
                           uint16_t number);

/// @brief Switch to view
/// @param view_manager view manager struct
/// @param number view's unique number
/// @param ctx user data passed to view init function
/// @return 0 if successfull
int view_manager_switch_view(ViewManager *view_manager, uint16_t number,
                             void *ctx);

/// @brief Remove view from view manager
/// @param view_manager view manager struct
/// @param number view's unique number
void view_manager_remove_view(ViewManager *view_manager, uint16_t number);

/// @brief Free view manager
/// @param view_manager view manager struct
void view_manager_free(ViewManager *view_manager);

/// @brief Initialize basic list view
/// @param view_manager view manager struct
/// @param elements list elements
/// @param icons list icons
/// @param element_cnt number of list elements
/// @param labels numbers of text elements
/// @param labels_cnt length of numbers array
/// @param event_handler input event handler
/// @return pointer to lvgl list
lv_obj_t *view_manager_list_init(ViewManager *view_manager, char **elements,
                                 const void **icons, uint32_t element_cnt,
                                 int *labels, int labels_cnt,
                                 void (*event_handler)(lv_event_t *));
