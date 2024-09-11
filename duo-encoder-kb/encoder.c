#include "encoder.h"

InputPins pin[PINS_NUM] = {ENC_A_NUM, ENC_B_NUM, BTN_1_NUM, BTN_2_NUM};
char edge[PINS_NUM][8] = {"both", "none", "both", "both"};

#ifndef PLUGIN
bool running;
#endif

bool readPin(int pin) {
  char value = 0;
  lseek(pin, 0, 0);
  read(pin, &value, 1);
  return value - 48;
}

void input_queue_input(InputQueue *queue, InputKey key, InputType type) {
  pthread_mutex_lock(&queue->mutex);
  Input input = {key, type};
  uint8_t size = queue->QueueSize;
  if (size < INPUT_QUEUE_SIZE_MAX - 1) {
    queue->data[size] = input;
    queue->QueueSize++;
  }
  pthread_mutex_unlock(&queue->mutex);
}

Input input_queue_read(InputQueue *queue) {
  pthread_mutex_lock(&queue->mutex);
  Input input;
  uint8_t size = queue->QueueSize;
  if (size > 0) {
    input = queue->data[0];
    for (uint8_t i = 0; i < size - 1; i++) {
      queue->data[i] = queue->data[i + 1];
    }
    queue->QueueSize--;
  }
  pthread_mutex_unlock(&queue->mutex);
  return input;
}

void *encoder_interrupt(void *ctx) {
  EncoderData *data = ctx;
  uint8_t encoder_A;
  uint8_t encoder_B;

  while (*data->running) {
    int n = poll(&data->poll_gpio[0], 3, 200);

    if (data->poll_gpio[0].revents & POLLPRI) {
      encoder_A = readPin(data->enc_A);
      encoder_B = readPin(data->enc_B);
      if (encoder_A != data->encoder_A_prev) {
        uint8_t direction = encoder_A == encoder_B;
        // printf("Direction: %d\n", direction);
        input_queue_input(data->inputQueue[INPUT_DEVICE_ENCODER], direction,
                          INPUT_TYPE_PRESS);
      }
      data->encoder_A_prev = encoder_A;
    }
    // Button debouncing time
    struct timespec rem, req = {0, 50 * 1000 * 1000};

    for (uint8_t i = 0; i < 2; i++) {
      bool pos = readPin(data->button[i]);
      if (data->poll_gpio[1 + i].revents & POLLPRI) {
        nanosleep(&req, &rem);
        if (readPin(data->button[i]) == pos) {
          input_queue_input(data->inputQueue[INPUT_DEVICE_BUTTONS], 2 + i, pos);
        }
      }
    }
  }

  return 0;
}

int encoder_init(EncoderData *data) {
  char command[64];

  for (uint8_t i = 0; i < PINS_NUM; i++) {
    snprintf(command, 64, "echo %d > /sys/class/gpio/export", pin[i]);
    system(command);
  }

  for (uint8_t i = 0; i < PINS_NUM; i++) {
    snprintf(command, 64, "echo %s > /sys/class/gpio/gpio%d/edge", edge[i],
             pin[i]);
    system(command);
  }
  char path[64];

  snprintf(path, 64, "/sys/class/gpio/gpio%d/value", ENC_A_NUM);
  data->enc_A = open(path, O_RDONLY | O_NONBLOCK);
  snprintf(path, 64, "/sys/class/gpio/gpio%d/value", ENC_B_NUM);
  data->enc_B = open(path, O_RDONLY | O_NONBLOCK);
  snprintf(path, 64, "/sys/class/gpio/gpio%d/value", BTN_1_NUM);
  data->button[0] = open(path, O_RDONLY | O_NONBLOCK);
  snprintf(path, 64, "/sys/class/gpio/gpio%d/value", BTN_2_NUM);
  data->button[1] = open(path, O_RDONLY | O_NONBLOCK);

  data->poll_gpio[0].fd = data->enc_A;
  data->poll_gpio[1].fd = data->button[0];
  data->poll_gpio[2].fd = data->button[1];
  for (uint8_t i = 0; i < 3; i++) {
    data->poll_gpio[i].events = POLLPRI;
    data->poll_gpio[i].revents = 0;
    readPin(data->poll_gpio[i].fd); // Dummy read
  }

  for (uint8_t i = 0; i < 2; i++) {
    data->inputQueue[i] = calloc(1, sizeof(*data->inputQueue[i]));
    if (pthread_mutex_init(&data->inputQueue[i]->mutex, NULL) != 0) {
      printf("Can't init mutex\n");
      return 1;
    }
  }

  if (!data->running) {
    printf("No running flag!\n");
    return 1;
  }
  *data->running = true;

  return 0;
}

void encoder_deinit(EncoderData *data) {
  *data->running = false;

  if (data->enc_A)
    close(data->enc_A);
  if (data->enc_B)
    close(data->enc_B);
  if (data->button[0])
    close(data->button[0]);
  if (data->button[1])
    close(data->button[1]);

  char command[64];

  for (uint8_t i = 0; i < PINS_NUM; i++) {
    snprintf(command, 64, "echo none > /sys/class/gpio/gpio%d/edge", pin[i]);
    system(command);
  }

  for (uint8_t i = 0; i < PINS_NUM; i++) {
    snprintf(command, 64, "echo %d > /sys/class/gpio/unexport", pin[i]);
    system(command);
  }
  for (uint8_t i = 0; i < 2; i++) {
    pthread_mutex_destroy(&data->inputQueue[i]->mutex);
    free(data->inputQueue[i]);
  }
}

int encoder_grab(EncoderData *data) {
  if (encoder_init(data) != 0)
    return 1;
  if (pthread_create(&data->encoder_pth, NULL, encoder_interrupt, data) != 0)
    return 1;
  return 0;
}

void encoder_release(EncoderData *data) {
  *data->running = false;
  pthread_join(data->encoder_pth, NULL);
  encoder_deinit(data);
}

#ifndef PLUGIN

static void sig_handler(int _) { running = false; }

int main(void) {
  EncoderData *data = calloc(1, sizeof(*data));
  data->running = &running;
  struct timespec rem, req = {0, 200 * 1000 * 1000};
  signal(SIGINT, sig_handler);

  if (encoder_grab(data) != 0) {
    printf("Main: init failure\n");
    return 1;
  }

  nanosleep(&req, &rem);
  // printf("Main: releasing encoder\n");
  // encoder_release(data);
  // nanosleep(&req, &rem);

  // system("./test");

  // if (encoder_grab(data) != 0)
  // {
  //   printf("Main: encoder grab failure\n");
  //   return 1;
  // }

  // printf("Main: encoder grabbed successfully\n");
  Input input;
  while (running) {
    if (data->inputQueue[INPUT_DEVICE_ENCODER]->QueueSize > 0) {
      input = input_queue_read(data->inputQueue[INPUT_DEVICE_ENCODER]);
      printf("Main: encoder %d\n", input.key);
    }
    if (data->inputQueue[INPUT_DEVICE_BUTTONS]->QueueSize > 0) {
      input = input_queue_read(data->inputQueue[INPUT_DEVICE_BUTTONS]);
      printf("Main: buttons %d %s\n", input.key, input.type ? "up" : "down");
    }
    nanosleep(&req, &rem);
  }

  encoder_release(data);
  free(data);
  printf("\nMain: exit\n");
  return 0;
}

#endif
