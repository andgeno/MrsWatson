#include "base/PlatformUtilities.h"

#include <stdlib.h>
#include <math.h>
#if UNIX
#include <unistd.h>
#include <time.h>
#endif

#include "unit/TestRunner.h"
#include "time/TaskTimer.h"

#define SLEEP_DURATION_MS 10.0
// Timer testing is a bit unreliable, so we just check to see that each sleep
// call (see below) is recorded off no more than this amount of milliseconds.
#define MAX_TIMER_TOLERANCE_MS 1.5f
#define TEST_COMPONENT_NAME "component"
#define TEST_SUBCOMPONENT_NAME "subcomponent"

static int _testNewTaskTimer(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, TEST_SUBCOMPONENT_NAME);

  assert(t->enabled);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, TEST_SUBCOMPONENT_NAME);
  assertDoubleEquals(t->totalTaskTime, 0.0, TEST_FLOAT_TOLERANCE);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithEmptyComponent(void) {
  CharString c = newCharStringWithCString(EMPTY_STRING);
  TaskTimer t = newTaskTimer(c, TEST_SUBCOMPONENT_NAME);

  assertNotNull(t);
  assertCharStringEquals(t->component, EMPTY_STRING);
  assertCharStringEquals(t->subcomponent, TEST_SUBCOMPONENT_NAME);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithEmptySubcomponent(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, EMPTY_STRING);

  assertNotNull(t);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, EMPTY_STRING);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithNullSubcomponent(void) {
  CharString c = newCharStringWithCString(TEST_COMPONENT_NAME);
  TaskTimer t = newTaskTimer(c, NULL);

  assertNotNull(t);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, EMPTY_STRING);

  freeCharString(c);
  freeTaskTimer(t);
  return 0;
}

static int _testNewObjectWithCStrings(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);

  assert(t->enabled);
  assertCharStringEquals(t->component, TEST_COMPONENT_NAME);
  assertCharStringEquals(t->subcomponent, TEST_SUBCOMPONENT_NAME);
  assertDoubleEquals(t->totalTaskTime, 0.0, TEST_FLOAT_TOLERANCE);

  freeTaskTimer(t);
  return 0;
}

static void _testSleep(void) {
#if UNIX
  struct timespec sleepTime;
  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = 1000000 * SLEEP_DURATION_MS;
  nanosleep(&sleepTime, NULL);
#elif WINDOWS
  Sleep((DWORD)SLEEP_DURATION_MS);
#endif
}

static int _testTaskTimerDuration(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);

  taskTimerStart(t);
  _testSleep();
  taskTimerStop(t);
  assertDoubleEquals(t->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);

  freeTaskTimer(t);
  return 0;
}

static int _testTaskTimerDurationMultipleTimes(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);
  int i;

  for(i = 0; i < 5; i++) {
    taskTimerStart(t);
    _testSleep();
    taskTimerStop(t);
  }
  assertDoubleEquals(t->totalTaskTime, 5.0 * SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS * 5.0);

  freeTaskTimer(t);
  return 0;
}

static int _testTaskTimerCallStartTwice(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);
  int i;

  taskTimerStart(t);
  taskTimerStart(t);
  _testSleep();
  taskTimerStop(t);
  assertDoubleEquals(t->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);

  freeTaskTimer(t);
  return 0;
}

static int _testTaskTimerCallStopTwice(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);
  int i;

  taskTimerStart(t);
  _testSleep();
  taskTimerStop(t);
  taskTimerStop(t);
  assertDoubleEquals(t->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);

  freeTaskTimer(t);
  return 0;
}

static int _testCallStopBeforeStart(void) {
  TaskTimer t = newTaskTimerWithCString(TEST_COMPONENT_NAME, TEST_SUBCOMPONENT_NAME);
  int i;

  taskTimerStop(t);
  taskTimerStart(t);
  _testSleep();
  taskTimerStop(t);
  assertDoubleEquals(t->totalTaskTime, SLEEP_DURATION_MS, MAX_TIMER_TOLERANCE_MS);

  freeTaskTimer(t);
  return 0;
}

TestSuite addTaskTimerTests(void);
TestSuite addTaskTimerTests(void) {
  TestSuite testSuite = newTestSuite("TaskTimer", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewTaskTimer);
  addTest(testSuite, "NewObjectWithEmptyComponent", _testNewObjectWithEmptyComponent);
  addTest(testSuite, "NewObjectWithEmptySubcomponent", _testNewObjectWithEmptySubcomponent);
  addTest(testSuite, "NewObjectWithNullSubcomponent", _testNewObjectWithNullSubcomponent);
  addTest(testSuite, "NewObjectWithCStrings", _testNewObjectWithCStrings);
  addTest(testSuite, "TaskDuration", _testTaskTimerDuration);
  addTest(testSuite, "TaskDurationMultipleTimes", _testTaskTimerDurationMultipleTimes);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopTwice", _testTaskTimerCallStopTwice);
  addTest(testSuite, "CallStartTwice", _testTaskTimerCallStartTwice);
  addTest(testSuite, "CallStopBeforeStart", _testCallStopBeforeStart);
  return testSuite;
}
