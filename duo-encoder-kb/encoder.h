#pragma once

#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUT_QUEUE_SIZE_MAX 4
#define LONG_PRESS_TIME 350  // Milliseconds

typedef enum
{
  ENC_A_NUM = 502,
  ENC_B_NUM = 505,
  BTN_1_NUM = 507,
  BTN_2_NUM = 506,

  PINS_NUM = 4,
} InputPins;

typedef enum
{
  INPUT_DEVICE_ENCODER,
  INPUT_DEVICE_BUTTONS,
} InputDevice;

typedef enum
{
  INPUT_KEY_UP,
  INPUT_KEY_DOWN,
  INPUT_KEY_OK,
  INPUT_KEY_BACK,

  INPUT_KEY_MAX,
} InputKey;

typedef enum
{
  INPUT_TYPE_PRESS,
  INPUT_TYPE_RELEASE,

  INPUT_TYPE_MAX,
} InputType;

typedef struct
{
  InputKey key;
  InputType type;
} Input;

typedef struct
{
  Input data[INPUT_QUEUE_SIZE_MAX];  // Stored input data
  uint8_t QueueSize;                 // Current queue size
  pthread_mutex_t mutex;             // Queue mutex
} InputQueue;

typedef struct
{
  int enc_A;                   // File descriptor for ENC_A pin
  int enc_B;                   // File descriptor for ENC_B pin
  int button[2];               // File descriptors for button pins
  uint8_t encoder_A_prev;      // Last ENC_A value
  struct pollfd poll_gpio[3];  // Gpio poll events
  bool* running;               // Interrupt thread running flag
  pthread_t encoder_pth;       // Encoder thread identifier
  InputQueue* inputQueue[2];   // Input queues
} Data;

// From <time.h>
int nanosleep(const struct timespec* req, struct timespec* rem);

/// @brief Generate input event
/// @param data Data struct
/// @param key Input key
/// @param type Input type
void input_queue_input(InputQueue* queue, InputKey key, InputType type);

/// @brief Read input event from queue
/// @param data Data struct
/// @return Input event
Input input_queue_read(InputQueue* queue);

/// @brief Initialize input
/// @param data Data struct
/// @return 0 if successful
int init(Data* data);

/// @brief Deinitialize input
/// @param data Data struct
void deinit(Data* data);

/// @brief Grab input
/// @param data Data struct
/// @return 0 if successful
int encoder_grab(Data* data);

/// @brief Release input
/// @param data Data struct
void encoder_release(Data* data);
