#pragma once

#include "encoder.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/lvgl.h"

typedef struct {
  void (*init)(void *);
  void (*exit)();
} View;

typedef struct {
  View **view;
  uint8_t current_view;
  lv_obj_t *obj_parent;
  uint8_t view_count;
} ViewManager;

/// @brief Initialize view manager
/// @return Pointer to view manager struct
ViewManager *view_manager_init(uint8_t view_count);

/// @brief Add view to the view manager
/// @param view_manager view manager struct
/// @param init init function of the view
/// @param input input function of the view
/// @param exit exit function of the view
/// @param number view's unique number
void view_manager_add_view(ViewManager *view_manager, void (*init)(void *),
                           void (*exit)(), uint8_t number);

/// @brief Switch to view
/// @param view_manager view manager struct
/// @param number view's unique number
/// @return 0 if successfull
int view_manager_switch_view(ViewManager *view_manager, uint8_t number);

/// @brief Remove view from view manager
/// @param view_manager view manager struct
/// @param number view's unique number
void view_manager_remove_view(ViewManager *view_manager, uint8_t number);

/// @brief Free view manager
/// @param view_manager view manager struct
void view_manager_free(ViewManager *view_manager);
