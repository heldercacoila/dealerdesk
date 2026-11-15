#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int g_tests_run = 0;
static int g_tests_failed = 0;
static const char *g_current_test = NULL;

#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        printf("  [FAIL] %s (%s:%d): Assertion failed: %s\n", \
               g_current_test ? g_current_test : "test", __FILE__, __LINE__, #condition); \
        g_tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_INT_EQ(expected, actual) do { \
    int _exp = (int)(expected); \
    int _act = (int)(actual); \
    if (_exp != _act) { \
        printf("  [FAIL] %s (%s:%d): Expected %d, got %d\n", \
               g_current_test ? g_current_test : "test", __FILE__, __LINE__, _exp, _act); \
        g_tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_LONG_EQ(expected, actual) do { \
    long _exp = (long)(expected); \
    long _act = (long)(actual); \
    if (_exp != _act) { \
        printf("  [FAIL] %s (%s:%d): Expected %ld, got %ld\n", \
               g_current_test ? g_current_test : "test", __FILE__, __LINE__, _exp, _act); \
        g_tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_DOUBLE_EQ(expected, actual) do { \
    double _exp = (double)(expected); \
    double _act = (double)(actual); \
    if (fabs(_exp - _act) > 0.001) { \
        printf("  [FAIL] %s (%s:%d): Expected %.2f, got %.2f\n", \
               g_current_test ? g_current_test : "test", __FILE__, __LINE__, _exp, _act); \
        g_tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_STR_EQ(expected, actual) do { \
    const char *_exp = (expected); \
    const char *_act = (actual); \
    if (!_exp || !_act || strcmp(_exp, _act) != 0) { \
        printf("  [FAIL] %s (%s:%d): Expected \"%s\", got \"%s\"\n", \
               g_current_test ? g_current_test : "test", __FILE__, __LINE__, \
               _exp ? _exp : "(null)", _act ? _act : "(null)"); \
        g_tests_failed++; \
        return; \
    } \
} while (0)

#define ASSERT_STR_CONTAINS(haystack, needle) do { \
    const char *_haystack = (haystack); \
    const char *_needle = (needle); \
    if (!_haystack || !_needle || strstr(_haystack, _needle) == NULL) { \
        printf("  [FAIL] %s (%s:%d): String did not contain \"%s\"\n", \
               g_current_test ? g_current_test : "test", __FILE__, __LINE__, \
               _needle ? _needle : "(null)"); \
        g_tests_failed++; \
        return; \
    } \
} while (0)

#define RUN_TEST(test_func) do { \
    int _before = g_tests_failed; \
    g_current_test = #test_func; \
    g_tests_run++; \
    test_func(); \
    if (g_tests_failed == _before) { \
        printf("  [PASS] %s\n", #test_func); \
    } \
} while (0)

#define TEST_REPORT() do { \
    printf("\n--------------------------------------------------------------\n"); \
    printf("Test Results: %d run, %d passed, %d failed\n", \
           g_tests_run, g_tests_run - g_tests_failed, g_tests_failed); \
    printf("--------------------------------------------------------------\n"); \
    return (g_tests_failed == 0) ? 0 : 1; \
} while (0)

#endif
