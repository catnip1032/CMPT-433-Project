#include "../include/classifierModule.h"
#include "../include/colorSensor.h"
#include "../include/timing.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
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

  printf("\nWaiting for ball to appear...\n");
  ClassifierModule_waitUntilBallAppears();

  eClassifierModule_BallColor ballColor = ClassifierModule_getBallColor();
  printf("Ball has appeared! Color is: ");
  if (ballColor == CLASSIFIER_MODULE_RED_BALL) {
    printf("Color: Red\n");
  }
  else if (ballColor == CLASSIFIER_MODULE_GREEN_BALL) {
    printf("Color: Green\n");
  }
  else {
    printf("Color: Blue\n");
  }

  ClassifierModule_cleanup();
}