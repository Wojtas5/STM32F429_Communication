from inspect import *
import datetime as dt

suiteName = ""
testName = ""
suites = 0
tests = 0
alltests = 0

failed = False
passedTests = 0
failedTests = 0
failedTestsNames = []

starttime = 0
totalTime = 0


def TEST(suite, test):
    global suiteName, testName
    global suites, tests, alltests
    global starttime, totalTime

    if test != testName and tests != 0:
        TEST_SUMMARY(suite)

    if suite != suiteName:
        suites += 1
        tests = 0

    suiteName = suite
    testName = test
    tests += 1
    alltests += 1

    print("[ RUN      ] {0}.{1}".format(suiteName, testName))
    starttime = dt.datetime.now()


def TEST_SUMMARY(suite):
    global starttime, totalTime, failed
    global failedTests, passedTests, failedTestsNames

    time = ((dt.datetime.now() - starttime).total_seconds() * 1000)
    totalTime += time

    if not failed:
        print("[       OK ] {0}.{1} ({2} ms)".format(suiteName, testName, round(time, 2)))
        passedTests += 1
    else:
        print("[  FAILED  ] {0}.{1} ({2} ms)".format(suiteName, testName, round(time, 2)))
        failedTests += 1
        failedTestsNames.append('{0}.{1}'.format(suiteName, testName))

    if suite != suiteName:
        time = ((dt.datetime.now() - starttime).total_seconds() * 1000)
        print("[----------] {0} tests from {1} ({2} ms total)".format(tests, suiteName, round(time, 2)))

    failed = False


def SUMMARY():
    TEST_SUMMARY('')
    print("[----------] Global test environment tear-down")
    print("[==========] {0} tests from {1} test suites ran. ({2} ms total)".format(alltests, suites, round(totalTime, 2)))
    print("[  PASSED  ] {0} tests.".format(passedTests))
    print("[  FAILED  ] {0} tests, listed below:".format(failedTests))
    for name in failedTestsNames:
        print("[  FAILED  ] {0}".format(name))


def EXPECT_EQ(val1, val2):
    global failed

    try:
        if val1 == val2:
            pass
        else:
            failed = True
            raise ValueError

    except ValueError:
        # Get line number of a function call from a module where this function was called
        line = currentframe().f_back.f_lineno

        # Retrieve original arguments
        frame = getouterframes(currentframe())[1]
        string = getframeinfo(frame[0]).code_context[0].strip()
        args = string[string.find('(') + 1:-1].split(',')

        # Print error message
        print("{0}:{1}: Failure".format(getframeinfo(currentframe().f_back).filename, line))
        print("Expected equality of these values:\n  {0}\n {1}\n  Which is: {2}".format(args[0], args[1], val2))


def EXPECT_NE(val1, val2):
    global failed

    try:
        if val1 != val2:
            pass
        else:
            failed = True
            raise ValueError

    except ValueError:
        # Get line number of a function call from a module where this function was called
        line = currentframe().f_back.f_lineno

        # Retrieve original arguments
        frame = getouterframes(currentframe())[1]
        string = getframeinfo(frame[0]).code_context[0].strip()
        args = string[string.find('(') + 1:-1].split(',')

        # Print error message
        print("{0}:{1}: Failure".format(getframeinfo(currentframe().f_back).filename, line))
        print("Expected: ({0}) != ({1}), actual: {2} vs {3}".format(args[0], args[1], val1, val2))


print('[----------] Global test environment set-up.')