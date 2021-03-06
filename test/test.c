#include "../include/classifierModule.h"
#include "../include/colorSensor.h"
#include "../include/led.h"
#include "../include/lights.h"
#include "../include/timing.h"
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Test definitions
// ----------------------------------------------------------------------------
typedef struct {
  char *testName;
  void (*test_func)(void);
} test_t;

#define TEST_EXAMPLE "testExample"
static void Test_testExample(void);

#define TEST_COLOR_SENSOR "testColorSensor"
static void Test_testColorSensor(void);

#define TEST_CLASSIFIER_MODULE "testClassifierModule"
static void Test_testClassifierModule(void);

#define TEST_LED "testLed"
static void Test_testLed(void);

#define TEST_LIGHTS "testLights"
static void Test_testLights(void);

// Do not modify this one. This will help the program determine that the end
// of tests has been reached.
#define END_OF_TESTS_STR "0_END_OF_TESTS"
static bool Test_reachedEndOfTests(test_t _test);
static test_t end_of_tests = {END_OF_TESTS_STR, NULL};

// Main and auxiliary function implementations
// ----------------------------------------------------------------------------
/* The way tests works is as follows: if you call this program with no
 * arguments, it will run all existing tests. If you call it with one or more
 * arguments, if the first argument correspond to a test string, then that
 * corresponding test function will be invoked. Otherwise, nothing happens. It
 * is a simple unitary test program that is not very flexible, but does the
 * job.
 */
int main(int argc, char *argv[])
{
  /* Add the corresponding test argument to the corresponding test function in
   * the array below (e.g. {TEST_MODULE, &Test_testModule}). Define test
   * strings in the Test Strings definitions above. The signature of test
   * functions should be f: void -> void. You are encouraged to include printf
   * statements, asserts, and exits with error in the test functions.
   */
  test_t tests[] = {{TEST_EXAMPLE, &Test_testExample},
                    {TEST_COLOR_SENSOR, &Test_testColorSensor},
                    {TEST_CLASSIFIER_MODULE, &Test_testClassifierModule},
                    {TEST_LED, &Test_testLed},
                    {TEST_LIGHTS, &Test_testLights},
                    end_of_tests};

  printf("Tests have started\n");
  if (argc <= 1) {
    int i = 0;
    test_t test = tests[i++];
    while (!Test_reachedEndOfTests(test)) {
      test.test_func();
      test = tests[i++];
    }
  }
  else {
    char *targetTest = argv[1];
    int i = 0;
    test_t test = tests[i++];
    while (!Test_reachedEndOfTests(test)) {
      if (strcmp(targetTest, test.testName) == 0) {
        test.test_func();
      }
      test = tests[i++];
    }
  }
}

static bool Test_reachedEndOfTests(test_t _test)
{
  return (strcmp(_test.testName, END_OF_TESTS_STR) == 0);
}

// Tests
// ----------------------------------------------------------------------------
static void Test_testExample(void)
{
  printf("This is a test example. It can include function calls, asserts, and \
much more! This one does nothing.");
  return;
}

static void Test_testColorSensor(void)
{
  static const int32_t NUM_COLOR_SENSOR_TEST_READS = 30;
  static const int64_t COLOR_READ_TIME_INTERVAL_NS = 750000000;

  printf("\nInitializing color sensor...\n");
  ColorSensor_init(2);
  for (int32_t i = 0; i < NUM_COLOR_SENSOR_TEST_READS; ++i) {
    int luminanceValues[5];
    ColorSensor_getLuminanceValuesInLux(luminanceValues);
    printf("Red luminance: %d lux\n", luminanceValues[0]);
    printf("Blue luminance: %d lux\n", luminanceValues[1]);
    printf("Green luminance: %d lux\n", luminanceValues[2]);
    printf("IR luminance: %d lux\n", luminanceValues[3]);
    printf("Ambient light luminance: %d lux\n", luminanceValues[4]);

    int rgbValues[3];
    ColorSensor_getRgbValues(rgbValues);
    printf("RGB: %d, %d, %d\n", rgbValues[0], rgbValues[1], rgbValues[2]);

    eColorSensorColor detectedColor = ColorSensor_getColor();
    if (detectedColor == COLOR_SENSOR_RED) {
      printf("Color: Red\n");
    }
    else if (detectedColor == COLOR_SENSOR_GREEN) {
      printf("Color: Green\n");
    }
    else {
      printf("Color: Blue\n");
    }

    bool isObjectInFrontOfColorSensor = ColorSensor_isObjectInFrontOfSensor();
    printf("Is an object in front of the color sensor: %s\n",
           isObjectInFrontOfColorSensor ? "True" : "False");

    printf("\n");
    Timing_nanoSleep(0, COLOR_READ_TIME_INTERVAL_NS);
  }

  ColorSensor_cleanup();
}

static void Test_testClassifierModule(void)
{
  printf("\nInitializing classifier module...\n");
  ClassifierModule_init(2);

  printf("\nWaiting for refuse item to appear...\n");
  ClassifierModule_waitUntilRefuseItemAppears();

  eClassifierModule_RefuseItemType refuseType =
      ClassifierModule_getRefuseItemType();
  printf("A refuse item has appeared! it is: ");
  if (refuseType == CLASSIFIER_MODULE_GARBAGE) {
    printf("Garbage :(\n");
  }
  else if (refuseType == CLASSIFIER_MODULE_COMPOST) {
    printf("Compost :)\n");
  }
  else {
    printf("Recycling :)\n");
  }

  ClassifierModule_cleanup();
}

void Test_handleInterruptTestLed(int _signal)
{
  Led_cleanup();
  exit(EXIT_SUCCESS);
}

static void Test_testLed(void)
{
  struct sigaction action;
  struct sigaction oldAction;
  memset(&action, 0, sizeof(action));
  action.sa_handler = &Test_handleInterruptTestLed;
  sigaction(SIGINT, &action, &oldAction);

  printf("Initializing LED API.\n");
  Led_init();

  // Initializing LED 0
  Led_initLed(LED_0);
  Led_initLed(LED_1);
  Led_initLed(LED_2);
  Led_initLed(LED_3);

  for (uint8_t i = 0; i < NUMS_OF_LEDS; i++) {
    Led_setLight(i, false);
  }

  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < NUMS_OF_LEDS; j++) {
      Led_setLight(j, true);
      Timing_milliSleep(0, 300);
      Led_setLight(j, false);
    }
  }

  for (uint8_t i = 0; i < 5; i++) {
    for (int8_t j = NUMS_OF_LEDS - 1; j >= 0; j--) {
      Led_setLight(j, true);
      Timing_milliSleep(0, 300);
      Led_setLight(j, false);
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < NUMS_OF_LEDS; j++) {
      Led_setLight(j, true);
    }

    Timing_milliSleep(0, 500);

    for (uint8_t j = 0; j < NUMS_OF_LEDS; j++) {
      Led_setLight(j, false);
    }

    Timing_milliSleep(0, 500);
  }

  printf("Terminating LED API.\n");
  Led_cleanup();
}

static void Test_testLights(void)
{
  struct sigaction action;
  struct sigaction oldAction;
  memset(&action, 0, sizeof(action));
  action.sa_handler = &Test_handleInterruptTestLed;
  sigaction(SIGINT, &action, &oldAction);

  printf("Initializing lights.\n");
  Lights_init();

  printf("Entering idle mode.\n");
  Lights_setIdle();

  Timing_milliSleep(5, 0);

  printf("Entering recycling mode.\n");
  Lights_setRecycling();

  Timing_milliSleep(5, 0);

  printf("Entering recycled mode.\n");
  Lights_setRecycled();

  Timing_milliSleep(5, 0);

  printf("Entering returning mode.\n");
  Lights_setReturning();

  Timing_milliSleep(5, 0);

  printf("Finishing lights testing.\n");
  Lights_cleanup();
}
