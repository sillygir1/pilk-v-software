#include "file_manager.h"

#define DIR_LIST_LEN 128
#define DISP_BUF_SIZE (128 * 1024)

static lv_obj_t *list;
static ViewManager *view_manager;
static FileManagerData *fm_data;
static bool skip_dirs;

static bool running;
static lv_group_t *input_group;
static EncoderData *enc_data;
lv_timer_t *battery_timer;
lv_obj_t *battery_icon;
lv_obj_t *mode_label;
int adc_fd;

// Handle chosen option
static void dialog_cb(char *option) {
  // TODO
  printf("Dialog returned '%s'\n", option);
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    // printf("%s\n", button_text);
    if (strcmp(button_text, "Exit") == 0) {
      raise(SIGINT);
      return;
    }
    fm_data->filename = button_text;

    if (strcmp(fm_data->filename, "..") == 0) {
      if (strcmp(fm_data->dir, "/") == 0)
        return;
      storage_dir_up(fm_data->dir);
      file_manager_update_list();
      return;
    }
    if (storage_is_dir(fm_data->dir, fm_data->filename)) {
      storage_dir_down(fm_data->dir, fm_data->filename);
      file_manager_update_list();
      return;
    }

    ViewManagerDialog *vm_dialog = calloc(1, sizeof(*vm_dialog));
    static char *opts[3] = {"Copy", "Move", "Delete"};
    static const char *icons[3] = {LV_SYMBOL_COPY, LV_SYMBOL_CUT,
                                   LV_SYMBOL_TRASH};
    vm_dialog->title = button_text;
    vm_dialog->options = opts;
    vm_dialog->icons = icons;
    vm_dialog->options_num = 3;
    vm_dialog->cb = dialog_cb;

    printf("%s\n", fm_data->filename);
    view_manager_dialog(view_manager, vm_dialog);

  } else if (code == LV_EVENT_KEY &&
             lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
    raise(SIGINT);
  }
}

static void encoder_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Input input;
  if (enc_data->inputQueue[INPUT_DEVICE_ENCODER]->QueueSize > 0) {
    input = input_queue_read(enc_data->inputQueue[INPUT_DEVICE_ENCODER]);
    // printf("CB: encoder %s\n", input.key ? "down" : "up");
    switch (input.key) {
    case INPUT_KEY_UP:
      data->enc_diff = -1;
      break;
    case INPUT_KEY_DOWN:
      data->enc_diff = 1;
      break;
    default:
      printf("Input reading error\n");
      break;
    }
  }
}

static void buttons_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Input input;
  if (enc_data->inputQueue[INPUT_DEVICE_BUTTONS]->QueueSize > 0) {
    input = input_queue_read(enc_data->inputQueue[INPUT_DEVICE_BUTTONS]);
    // printf("CB: buttons %s\n", input.key == INPUT_KEY_BACK ? "back" :
    // "enter");
    switch (input.key) {
    case INPUT_KEY_BACK:
      data->key = LV_KEY_ESC;
      break;
    case INPUT_KEY_OK:
      data->key = LV_KEY_ENTER;
      break;
    default:
      printf("Input reading error\n");
      break;
    }
    data->state = input.type;
  }
}

void draw_status_bar() {
  battery_icon = lv_label_create(lv_scr_act());

  lv_label_set_text(battery_icon, "0\% \xEF\x89\x82");
  lv_obj_set_align(battery_icon, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_style_pad_right(battery_icon, 5, LV_PART_MAIN);

  lv_obj_set_y(battery_icon, 5);

  mode_label = lv_label_create(lv_scr_act());
  lv_label_set_text(mode_label, "");
  lv_obj_set_align(mode_label, LV_ALIGN_TOP_LEFT);
  lv_obj_set_style_pad_left(mode_label, 5, LV_PART_MAIN);

  lv_obj_set_y(mode_label, 5);
}

void set_mode_text(char *text) { lv_label_set_text(mode_label, text); }

void update_charge() {
  if (!adc_fd) {
    printf("ADC not initialized!\n");
    return;
  }
  char text[64];
  float voltage = read_voltage(adc_fd);
  char symbol[4];
  if (voltage > 4.1) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_FULL);
  } else if (voltage > 3.9) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_3);
  } else if (voltage > 3.7) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_2);
  } else if (voltage > 3.5) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_1);
  } else {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_EMPTY);
  }
  snprintf(text, 64, "%.01fV %s", voltage, symbol);
  if (battery_icon)
    lv_label_set_text(battery_icon, text);
}

static void battery_timer_cb(lv_timer_t *timer) {
  if (adc_fd && battery_icon)
    update_charge();
}

static void sig_handler(int _) { running = false; }

static void lvgl_init() {
  signal(SIGINT, sig_handler);

  printf("Gui initialization!\n");

  /*LittlevGL init*/
  lv_init();

  /*Linux frame buffer device init*/
  fbdev_init();

  /*A small buffer for LittlevGL to draw the screen's content*/
  static lv_color_t buf1[DISP_BUF_SIZE];
  static lv_color_t buf2[DISP_BUF_SIZE];

  /*Initialize a descriptor for the buffer*/
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

  /*Initialize and register a display driver*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = fbdev_flush;
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 320;
  lv_disp_drv_register(&disp_drv);

  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Loading");
  lv_obj_center(label);
  lv_timer_handler();
  lv_obj_del(label);

  /*Initialize and register an input drivers*/
  static lv_indev_drv_t encoder_drv;
  lv_indev_drv_init(&encoder_drv);
  encoder_drv.type = LV_INDEV_TYPE_ENCODER;
  encoder_drv.read_cb = encoder_cb;
  lv_indev_t *encoder_indev = lv_indev_drv_register(&encoder_drv);

  static lv_indev_drv_t buttons_drv;
  lv_indev_drv_init(&buttons_drv);
  buttons_drv.type = LV_INDEV_TYPE_KEYPAD;
  buttons_drv.read_cb = buttons_cb;
  lv_indev_t *buttons_indev = lv_indev_drv_register(&buttons_drv);

  input_group = lv_group_create();
  lv_group_set_default(input_group);
  lv_indev_set_group(encoder_indev, input_group);
  lv_indev_set_group(buttons_indev, input_group);
  enc_data = calloc(1, sizeof(*enc_data));
  enc_data->running = &running;
}

int main() {
  lvgl_init();

  adc_fd = adc_init();
  if (adc_fd < 0)
    printf("Adc init issue");

  encoder_grab(enc_data);

  battery_timer = lv_timer_create(battery_timer_cb, 2000, NULL);

  draw_status_bar();
  update_charge();

  view_manager = view_manager_init(1);
  lv_obj_set_scrollbar_mode(view_manager->obj_parent, LV_SCROLLBAR_MODE_OFF);
  view_manager_add_view(view_manager, file_manager_init, file_manager_exit, 1);
  fm_data = calloc(1, sizeof(*fm_data));
  fm_data->event_handler = event_handler;
  strcpy(fm_data->dir, "/");
  fm_data->file_type = 0;
  view_manager_switch_view(view_manager, 1, fm_data);

  draw_status_bar();
  update_charge();

  running = true;
  struct timespec rem, req = {0, 100 * 1000 * 1000};
  while (running) {
    lv_timer_handler();

    nanosleep(&req, &rem);
  }
  encoder_release(enc_data);
  lv_timer_del(battery_timer);
  lv_obj_clean(lv_scr_act());
  lv_timer_handler();
  view_manager_free(view_manager);
  close(adc_fd);
  free(enc_data);
  return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void) {
  static uint64_t start_ms = 0;
  if (start_ms == 0) {
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
  }

  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  uint64_t now_ms;
  now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

  uint32_t time_ms = now_ms - start_ms;
  return time_ms;
}
