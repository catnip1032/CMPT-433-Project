#include "../include/lights.h"

#include "../include/led.h"
#include "../include/timing.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

// Thread definitions
// ----------------------------------------------------------------------------
static pthread_t m_lightThread;
static bool m_threadActive;

void Light_startThread(void *(_func)(void *), void *_args);
void Light_stopThread(void);

typedef enum { NONE_MODE, MOVING_TAIL_MODE, BLINKING_MODE } eModes;
static eModes m_currentMode = NONE_MODE;

// Moving tail definitions
// ----------------------------------------------------------------------------
typedef enum { LIGHT_RIGHT, LIGHT_LEFT } eLightDirection;

typedef struct {
  uint8_t headNum;
  eLightDirection currentDirection;
  bool tailVisible;
} sTailConfig;

static void Light_initializeMovingTail(sTailConfig *_tailConfig);
static void Light_movingTailNextStepNoWrap(sTailConfig *_tailConfig);
static void Light_movingTailSetLeds(sTailConfig *_tailConfig);

static void *Light_movingTailThreadFunction(void *_args);
static void Light_startMovingTail(void);

#define IDLE_INTERVAL_MS 250
#define RECYCLING_INTERVAL_MS 125
static uint64_t m_movingTailInterval;

// Blink definitions
// ----------------------------------------------------------------------------

typedef struct {
  bool isOn;
} sBlinkConfig;

static void Light_initializeBlink(sBlinkConfig *_blinkConfig);
static void Light_blinkNextStep(sBlinkConfig *_blinkConfig);
static void Light_blinkSetLeds(sBlinkConfig *_blinkConfig);

static void *Light_blinkingThreadFunction(void *_args);
static void Light_startBlinking(void);

#define RECYCLED_BLINK_INTERVAL_MS 500
#define RETURNING_BLINK_INTERVAL_MS 250
static uint64_t m_blinkInterval;

// Thread implementations
// ----------------------------------------------------------------------------
void Light_startThread(void *(_func)(void *), void *_args)
{
  if (!m_threadActive) {
    m_threadActive = true;
    pthread_create(&m_lightThread, NULL, _func, _args);
  }
}

void Light_stopThread(void)
{
  if (m_threadActive) {
    m_threadActive = false;
    pthread_join(m_lightThread, NULL);
  }
}

// Moving tail function implementations
// ----------------------------------------------------------------------------
static void Light_initializeMovingTail(sTailConfig *_tailConfig)
{
  _tailConfig->headNum = 0;
  _tailConfig->currentDirection = LIGHT_RIGHT;
  _tailConfig->tailVisible = false;
}

static void Light_movingTailNextStepNoWrap(sTailConfig *_tailConfig)
{
  // If at rightmost led and moving to right
  if (_tailConfig->headNum == NUMS_OF_LEDS - 1 &&
      _tailConfig->currentDirection == LIGHT_RIGHT) {
    _tailConfig->currentDirection = LIGHT_LEFT;
    _tailConfig->tailVisible = false;
  }
  // If at leftmost led and moving to left
  else if (_tailConfig->headNum == 0 &&
           _tailConfig->currentDirection == LIGHT_LEFT) {
    _tailConfig->currentDirection = LIGHT_RIGHT;
    _tailConfig->tailVisible = false;
  }
  else if (_tailConfig->currentDirection == LIGHT_RIGHT) {
    _tailConfig->headNum++;
    _tailConfig->tailVisible = true;
  }
  else if (_tailConfig->currentDirection == LIGHT_LEFT) {
    _tailConfig->headNum--;
    _tailConfig->tailVisible = true;
  }
}

static void Light_movingTailSetLeds(sTailConfig *_tailConfig)
{
  for (uint8_t i = 0; i < NUMS_OF_LEDS; i++) {
    /*  Turns on the led if either LED i corresponds to the head of the "moving
        tail" or if it corresponds to the tail position and the tail is
        visible.
      */
    if (i == _tailConfig->headNum ||
        (i == _tailConfig->headNum - 1 &&
         _tailConfig->currentDirection == LIGHT_RIGHT &&
         _tailConfig->tailVisible) ||
        (i == _tailConfig->headNum + 1 &&
         _tailConfig->currentDirection == LIGHT_LEFT &&
         _tailConfig->tailVisible)) {
      Led_setLight(i, true);
    }
    else {
      Led_setLight(i, false);
    }
  }
}

static void *Light_movingTailThreadFunction(void *_args)
{
  sTailConfig tailConfig;
  Light_initializeMovingTail(&tailConfig);

  while (m_threadActive) {
    Light_movingTailSetLeds(&tailConfig);
    Timing_milliSleep(0, m_movingTailInterval);
    Light_movingTailNextStepNoWrap(&tailConfig);
  }

  return NULL;
}

static void Light_startMovingTail(void)
{
  Light_stopThread();
  m_currentMode = MOVING_TAIL_MODE;
  Light_startThread(&Light_movingTailThreadFunction, NULL);
}

// Blinking funtion implementations
// ----------------------------------------------------------------------------
static void Light_initializeBlink(sBlinkConfig *_blinkConfig)
{
  _blinkConfig->isOn = false;
}

static void Light_blinkNextStep(sBlinkConfig *_blinkConfig)
{
  _blinkConfig->isOn ^= 1;
}

static void Light_blinkSetLeds(sBlinkConfig *_blinkConfig)
{
  for (uint8_t i = 0; i < NUMS_OF_LEDS; i++) {
    Led_setLight(i, _blinkConfig->isOn);
  }
}

static void *Light_blinkingThreadFunction(void *_args)
{
  sBlinkConfig blinkConfig;
  Light_initializeBlink(&blinkConfig);

  while (m_threadActive) {
    Light_blinkSetLeds(&blinkConfig);
    Timing_milliSleep(0, m_blinkInterval);
    Light_blinkNextStep(&blinkConfig);
  }

  return NULL;
}

static void Light_startBlinking(void)
{
  Light_stopThread();
  m_currentMode = BLINKING_MODE;
  Light_startThread(&Light_blinkingThreadFunction, NULL);
}

// Initialization/termination functions
// ----------------------------------------------------------------------------
void Lights_init(void)
{
  Led_init();

  for (uint8_t i = 0; i < NUMS_OF_LEDS; i++) {
    Led_initLed(i);
  }
}

void Lights_cleanup(void)
{
  Light_stopThread();
  Led_cleanup();
}

// Light stages functions
// ----------------------------------------------------------------------------
void Lights_setIdle(void)
{
  if (m_currentMode != MOVING_TAIL_MODE) {
    Light_startMovingTail();
  }

  m_movingTailInterval = IDLE_INTERVAL_MS;
}

void Lights_setRecycling(void)
{
  if (m_currentMode != MOVING_TAIL_MODE) {
    Light_startMovingTail();
  }

  m_movingTailInterval = RECYCLING_INTERVAL_MS;
}

void Lights_setRecycled(void)
{
  if (m_currentMode != BLINKING_MODE) {
    Light_startBlinking();
  }

  m_blinkInterval = RECYCLED_BLINK_INTERVAL_MS;
}
void Lights_setReturning(void)
{
  if (m_currentMode != BLINKING_MODE) {
    Light_startBlinking();
  }

  m_blinkInterval = RETURNING_BLINK_INTERVAL_MS;
}
