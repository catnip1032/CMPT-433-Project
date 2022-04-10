#include "../include/led.h"
#include "../include/file.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_BUFFER_SIZE 1024
#define BRIGHTNESS_CHARS 4

// Led configuration definitions
// ----------------------------------------------------------------------------
typedef struct {
  char original_trigger[STR_BUFFER_SIZE];
  char original_brightness[STR_BUFFER_SIZE];
  bool isInitialized;
} sLedConfig;

sLedConfig m_ledsConfig[NUMS_OF_LEDS];
char m_maxBrightness[BRIGHTNESS_CHARS];

// String function definitions
// ----------------------------------------------------------------------------

static char *Led_makePath(char *_buffer, size_t _size, eLedNum _ledNum,
                          const char *_file);

// Writes the _ledNum trigger to buffer _outBuffer of size _size. Returns true
// if successful, and false otherwise.
static bool Led_getTrigger(eLedNum _ledNum, char *_outBuffer, size_t _size);
static bool Led_getBrightness(eLedNum _ledNum, char *_outBuffer, size_t _size);

// String function implementations
// ----------------------------------------------------------------------------
static char *Led_makePath(char *_buffer, size_t _size, eLedNum _ledNum,
                          const char *_file)
{
  snprintf(_buffer, _size, FILE_LED_PATH "%d%s", _ledNum, _file);
  return _buffer;
}

static bool Led_getTrigger(eLedNum _ledNum, char *_outBuffer, size_t _size)
{
  char buffer[STR_BUFFER_SIZE];
  char filepathBuffer[STR_BUFFER_SIZE];

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_TRIGGER);
  if (!File_readFromFile(filepathBuffer, buffer, STR_BUFFER_SIZE)) {
    fprintf(stderr, "Could not read from %s.\n", filepathBuffer);
    return false;
  }

  char *triggerBeginning = memchr(buffer, '[', STR_BUFFER_SIZE);
  if (!triggerBeginning) {
    fprintf(stderr, "Could not find trigger in %s.\n", filepathBuffer);
    fprintf(stderr, "File contents: %s\n", buffer);
    return false;
  }

  char *triggerEnding = memchr(buffer, ']', STR_BUFFER_SIZE);
  if (!triggerEnding) {
    fprintf(stderr, "Could not find end of trigger in %s.\n", filepathBuffer);
    fprintf(stderr, "File contents: %s\n", buffer);
    return false;
  }

  triggerBeginning++; // Move to first char in trigger string

  // Plus one to account for null character
  size_t triggerStrSize = sizeof(char) * (triggerEnding - triggerBeginning + 1);
  // printf("_size: %d, triggerStrSize: %d\n", _size, triggerStrSize);
  assert(_size > triggerStrSize);

  memcpy(_outBuffer, triggerBeginning, triggerStrSize - 1);
  _outBuffer[triggerStrSize] = '\0';
  return true;
}

static bool Led_getBrightness(eLedNum _ledNum, char *_outBuffer, size_t _size)
{
  assert(_size >= BRIGHTNESS_CHARS);

  char filepathBuffer[STR_BUFFER_SIZE];

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_BRIGHTNESS);
  if (!File_readFromFile(filepathBuffer, _outBuffer, _size)) {
    fprintf(stderr, "Could not read from %s.\n", filepathBuffer);
    return false;
  }

  return true;
}

// Initialization/Termination functions
// ----------------------------------------------------------------------------
void Led_init(void)
{
  char filepathBuffer[STR_BUFFER_SIZE];

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, LED_0, FILE_LED_MAX_BRIGHTNESS);
  assert(File_readFromFile(filepathBuffer, m_maxBrightness, BRIGHTNESS_CHARS));

  for (uint8_t i = 0; i < NUMS_OF_LEDS; i++) {
    m_ledsConfig[i].isInitialized = false;
  }
}

void Led_cleanup(void)
{
  for (uint8_t i = 0; i < NUMS_OF_LEDS; i++) {
    sLedConfig *ledConfig = m_ledsConfig + i;
    if (ledConfig->isInitialized) {
      Led_termLed(i);
    }
  }
}

void Led_initLed(eLedNum _ledNum)
{
  char filepathBuffer[STR_BUFFER_SIZE];
  sLedConfig *ledConfig = m_ledsConfig + _ledNum;
  Led_getTrigger(_ledNum, ledConfig->original_trigger, STR_BUFFER_SIZE);
  Led_getBrightness(_ledNum, ledConfig->original_brightness, STR_BUFFER_SIZE);

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_TRIGGER);
  File_writeToFile(filepathBuffer, "none");
  ledConfig->isInitialized = true;

  Led_setLight(_ledNum, false);
}

void Led_termLed(eLedNum _ledNum)
{
  char filepathBuffer[STR_BUFFER_SIZE];
  sLedConfig *ledConfig = m_ledsConfig + _ledNum;
  ledConfig->isInitialized = false;

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_TRIGGER);
  File_writeToFile(filepathBuffer, ledConfig->original_trigger);

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_BRIGHTNESS);
  File_writeToFile(filepathBuffer, ledConfig->original_brightness);
}

// Light functions
// ----------------------------------------------------------------------------
void Led_setLight(eLedNum _ledNum, bool _value)
{
  if (!Led_isInitialized(_ledNum)) {
    fprintf(stderr, "Trying to write to non initialized led %d\n", _ledNum);
    return;
  }

  char filepathBuffer[STR_BUFFER_SIZE];

  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_BRIGHTNESS);
  char *valueToWrite = _value ? m_maxBrightness : "0";
  if (!File_writeToFile(filepathBuffer, valueToWrite)) {
    fprintf(stderr, "It was not possible to write to %s.\n", filepathBuffer);
    return;
  }
}

bool Led_getLight(eLedNum _ledNum)
{
  char buffer[BRIGHTNESS_CHARS];
  char filepathBuffer[STR_BUFFER_SIZE];
  Led_makePath(filepathBuffer, STR_BUFFER_SIZE, _ledNum, FILE_LED_BRIGHTNESS);
  if (!File_readFromFile(filepathBuffer, buffer, BRIGHTNESS_CHARS)) {
    fprintf(stderr, "It was not possible to read from %s.\n", filepathBuffer);
    return false;
  }

  return atoi(buffer) == 0 ? false : true;
}

bool Led_isInitialized(eLedNum _ledNum)
{
  return m_ledsConfig[_ledNum].isInitialized;
}
