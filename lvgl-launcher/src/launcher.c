#include "launcher.h"
#include "views/view_list.h"

#define DISP_BUF_SIZE (128 * 1024)

bool running = true;
MenuData *main_menu;
ViewManager *view_manager;

static void encoder_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Input input;
  if (main_menu->enc_data->inputQueue[INPUT_DEVICE_ENCODER]->QueueSize > 0) {
    input =
        input_queue_read(main_menu->enc_data->inputQueue[INPUT_DEVICE_ENCODER]);
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
  if (main_menu->enc_data->inputQueue[INPUT_DEVICE_BUTTONS]->QueueSize > 0) {
    input =
        input_queue_read(main_menu->enc_data->inputQueue[INPUT_DEVICE_BUTTONS]);
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

void battery_timer(lv_timer_t *timer) { update_charge(); }

void lvgl_init() {
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

  main_menu = calloc(1, sizeof(*main_menu));
  main_menu->running = &running;

  main_menu->input_group = lv_group_create();
  lv_group_set_default(main_menu->input_group);
  lv_indev_set_group(encoder_indev, main_menu->input_group);
  lv_indev_set_group(buttons_indev, main_menu->input_group);
  main_menu->enc_data = calloc(1, sizeof(*main_menu->enc_data));
  main_menu->enc_data->running = &running;

  draw_status_bar();

  view_manager = view_manager_init(VIEW_COUNT);
  lv_obj_set_scrollbar_mode(view_manager->obj_parent, LV_SCROLLBAR_MODE_OFF);
  views_init(view_manager);
  view_manager_switch_view(view_manager, VIEW_MAIN_MENU, NULL);
}

static void sig_handler(int _) { running = false; }

int main(void) {
  system("psplash-write \"MSG Starting launcher\"");

  signal(SIGINT, sig_handler);
  lvgl_init();
  encoder_grab(main_menu->enc_data);

  system("psplash-write \"MSG Initializing ADC\"");
  main_menu->adc_fd = adc_init();
  if (main_menu->adc_fd < 0)
    printf("Adc init issue");
  system("psplash-write \"PROGRESS 100\"");

  update_charge();
  lv_timer_t *timer = lv_timer_create(battery_timer, 2000, main_menu);

  // system("psplash-write \"QUIT\"");

  struct timespec rem, req = {0, 100 * 1000 * 1000};
  while (running) {
    lv_timer_handler();

    nanosleep(&req, &rem);
  }

  encoder_release(main_menu->enc_data);
  lv_timer_del(timer);
  view_manager_free(view_manager);
  close(main_menu->adc_fd);
  free(main_menu->enc_data);
  free(main_menu);
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

void set_mode_text(char *text) {
  lv_label_set_text(main_menu->mode_label, text);
}

void draw_status_bar() {
  main_menu->battery_icon = lv_label_create(lv_scr_act());

  lv_label_set_text(main_menu->battery_icon, "0\% \xEF\x89\x82");
  lv_obj_set_align(main_menu->battery_icon, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_style_pad_right(main_menu->battery_icon, 5, LV_PART_MAIN);

  lv_obj_set_y(main_menu->battery_icon, 5);

  main_menu->mode_label = lv_label_create(lv_scr_act());
  // set_mode_text("Pilk-V Duo");
  lv_label_set_text(main_menu->mode_label, "Pilk-V Duo");
  lv_obj_set_align(main_menu->mode_label, LV_ALIGN_TOP_LEFT);
  lv_obj_set_style_pad_left(main_menu->mode_label, 5, LV_PART_MAIN);

  lv_obj_set_y(main_menu->mode_label, 5);
}

void update_charge() {
  char text[64];
  float voltage = read_voltage(main_menu->adc_fd);
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
  if (main_menu->battery_icon)
    lv_label_set_text(main_menu->battery_icon, text);
}

void launch_app(char *cmd) {
  if (strcmp(cmd, "") == 0)
    return;
  encoder_release(main_menu->enc_data);
  system(cmd);
  encoder_grab(main_menu->enc_data);
}