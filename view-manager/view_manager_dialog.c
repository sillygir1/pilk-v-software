#include "view_manager_dialog.h"
#include "view_manager_dialog_i.h"

static lv_obj_t *dialog_screen;
static ViewManager *view_manager;
static ViewManagerDialog *vm_dialog;
static ViewManagerDialogStack *dialog_stack;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  lv_obj_t *list = dialog_stack->dialog[dialog_stack->count - 1]->list;

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("Dialog: %s\n", button_text);
    vm_dialog->cb(button_text);

  } else if (code == LV_EVENT_KEY &&
             lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
    view_manager_dialog_exit();
  }
}

void view_manager_dialog(void *_view_manager, ViewManagerDialog *_vm_dialog) {
  view_manager = _view_manager;
  vm_dialog = _vm_dialog;
  if (vm_dialog->options_num == 0)
    return;

  if (dialog_stack == 0) {
    dialog_stack = calloc(1, sizeof(*dialog_stack));
  }
  if (dialog_stack->count >= DIALOGS_MAX) {
    printf("Max number of dialogs reached: %u\n", dialog_stack->count);
    return;
  }

  dialog_stack->dialog[dialog_stack->count] =
      calloc(1, sizeof(*dialog_stack->dialog[0]));

  lv_group_t **input_group =
      dialog_stack->dialog[dialog_stack->count]->input_group;
  lv_indev_t **enc_indev = dialog_stack->dialog[dialog_stack->count]->enc_indev;

  // Switching input group
  input_group[0] = lv_group_get_default();
  input_group[1] = lv_group_create();
  enc_indev[0] = lv_indev_get_next(NULL);
  enc_indev[1] = lv_indev_get_next(enc_indev[0]);
  lv_indev_set_group(enc_indev[0], input_group[1]);
  lv_indev_set_group(enc_indev[1], input_group[1]);
  lv_group_set_default(input_group[1]);

  lv_obj_t *list = lv_list_create(lv_scr_act());

  dialog_stack->dialog[dialog_stack->count]->list = list;
  lv_obj_set_width(list, 230);
  lv_obj_set_height(list, LV_SIZE_CONTENT);
  lv_obj_set_align(list, LV_ALIGN_CENTER);
  lv_obj_set_style_outline_width(list, 5, LV_PART_MAIN);
  lv_obj_set_style_outline_color(list, lv_color_make(0x69, 0x69, 0x69),
                                 LV_PART_MAIN);

  lv_obj_t *btn;
  lv_list_add_text(list, vm_dialog->title);
  for (int i = 0; i < vm_dialog->options_num; i++) {
    btn = lv_list_add_btn(list, vm_dialog->icons[i], vm_dialog->options[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  }

  if (dialog_stack->count < DIALOGS_MAX) {
    dialog_stack->count += 1;
  }
}

void view_manager_dialog_exit() {
  if (dialog_stack->count <= 0) {
    return;
  }
  dialog_stack->count -= 1;

  lv_group_t **input_group =
      dialog_stack->dialog[dialog_stack->count]->input_group;
  lv_indev_t **enc_indev = dialog_stack->dialog[dialog_stack->count]->enc_indev;
  lv_obj_t *list = dialog_stack->dialog[dialog_stack->count]->list;

  lv_obj_del(list);

  // Setting input groups back
  lv_indev_set_group(enc_indev[0], input_group[0]);
  lv_indev_set_group(enc_indev[1], input_group[0]);
  lv_group_del(input_group[1]);
  lv_group_set_default(input_group[0]);

  if (dialog_stack->count > 0) {
    free(dialog_stack->dialog[dialog_stack->count]);
    dialog_stack->dialog[dialog_stack->count] = NULL;
  } else {
    free(dialog_stack->dialog[dialog_stack->count]);
    free(dialog_stack);
    dialog_stack->dialog[dialog_stack->count] = NULL;
    dialog_stack = NULL;
  }
  if (vm_dialog) {
    free(vm_dialog);
    vm_dialog = NULL;
  }
}