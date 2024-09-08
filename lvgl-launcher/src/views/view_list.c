#include "view_list.h"
#include "view_manager.h"

#include "apps_screen_view.h"
#include "main_screen_view.h"

void views_init(void *_view_manager) {
  ViewManager *view_manager = _view_manager;

  // Add all views here
  view_manager_add_view(view_manager, &menu_init, &menu_exit, VIEW_MAIN_MENU);
  view_manager_add_view(view_manager, &apps_init, &apps_exit, VIEW_APPS);
}