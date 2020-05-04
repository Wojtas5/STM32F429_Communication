from inspect import *
import datetime as dt


class TEST:
    suites = []
    failedTestsNames = []
    passedTests = 0
    failedTests = 0
    failed = False

    times = []
    totalTime = 0
    suiteTime = 0
    starttime = 0

    def __new__(cls, suite, test):
        if cls.suites != []:
            time = ((dt.datetime.now() - cls.starttime).total_seconds() * 1000)
            cls.times.append(time)
        cls.suites.append([suite, test])
        EXPECT_EQ.itersAppend()
        EXPECT_NE.itersAppend()
        cls.starttime = dt.datetime.now()

    @classmethod
    def TEST_SUMMARY(cls, suite, test):
        idx = cls.suites.index([suite, test])
        time = cls.times[idx]
        cls.suiteTime += time
        cls.totalTime += time

        if not cls.failed:
            print("[       OK ] {0}.{1} ({2} ms)".format(suite, test, round(time, 2)))
            cls.passedTests += 1
        else:
            print("[  FAILED  ] {0}.{1} ({2} ms)".format(suite, test, round(time, 2)))
            cls.failedTests += 1
            cls.failedTestsNames.append('{0}.{1}'.format(suite, test))

        if idx+1 < len(cls.suites):
            nextsuite = cls.suites[idx+1][0]
        else:
            nextsuite = ''  # There is no next suites, so we need a value to pass the next if statement

        if suite != nextsuite:
            print("[----------] {0} tests from {1} ({2} ms total)".format(cls.testsQuantity(suite),
                                                                          suite, round(cls.suiteTime, 2)))
            cls.suiteTime = 0

        cls.failed = False

    @classmethod
    def SUMMARY(cls):
        print("[==========] {0} tests from {1} test suites ran. ({2} ms total)".format(len(cls.suites),
                                                                                       cls.suitesQuantity(),
                                                                                       round(cls.totalTime, 2)))
        print("[  PASSED  ] {0} tests.".format(cls.passedTests))
        if cls.failedTests != 0:
            print("[  FAILED  ] {0} tests, listed below:".format(cls.failedTests))
        else:
            print("[  FAILED  ] {0} tests.".format(cls.failedTests))

        for name in cls.failedTestsNames:
            print("[  FAILED  ] {0}".format(name))

    @classmethod
    def testsQuantity(cls, suite):
        tests = 0
        for s, t in cls.suites:
            if s == suite:
                tests += 1
        return tests

    @classmethod
    def suitesQuantity(cls):
        suites = 0
        prev = ''
        for s, t in cls.suites:
            if s != prev:
                suites += 1
            prev = s
        return suites

    @classmethod
    def EXPECT_EQ(cls, val1, val2, line):
        try:
            if val1 == val2:
                pass
            else:
                cls.failed = True
                raise ValueError

        except ValueError:
            # Retrieve original arguments
            frame = getframeinfo(currentframe().f_back.f_back)
            f = open(frame.filename)
            for i, c in enumerate(f):
                if i == line - 1:
                    content = c
            f.close()

            args = content[content.find('(') + 1:-2].split(', ')

            # Print error message
            print("{0}:{1}: Failure".format(getframeinfo(currentframe().f_back).filename, line))
            print("Expected equality of these values:\n  {0}\n  {1}\n  Which is: {2}".format(args[0], args[1], val2))

    @classmethod
    def EXPECT_NE(cls, val1, val2, line):
        try:
            if val1 != val2:
                pass
            else:
                cls.failed = True
                raise ValueError

        except ValueError:
            # Retrieve original arguments
            frame = getframeinfo(currentframe().f_back.f_back)
            f = open(frame.filename)
            for i, c in enumerate(f):
                if i == line - 1:
                    content = c
            f.close()

            args = content[content.find('(') + 1:-2].split(', ')

            # Print error message
            print("{0}:{1}: Failure".format(frame.filename, line))
            print("Expected: ({0}) != ({1}), actual: {2} vs {3}".format(args[0], args[1], val1, val2))

    @classmethod
    def main(cls):
        # setUp()
        time = ((dt.datetime.now() - cls.starttime).total_seconds() * 1000)
        cls.times.append(time)
        print("[==========] Running {0} tests from {1} test suites".format(len(cls.suites), cls.suitesQuantity()))
        EQ = 0
        NE = 0

        for tests in range(len(cls.suites)):
            suite, test = cls.suites[tests]
            print("[ RUN      ] {0}.{1}".format(suite, test))

            for i in range(EXPECT_EQ.iters[tests]):
                val1, val2 = EXPECT_EQ.values[EQ]
                cls.EXPECT_EQ(val1, val2, EXPECT_EQ.lines[EQ])
                EQ += 1

            for i in range(EXPECT_NE.iters[tests]):
                val1, val2 = EXPECT_NE.values[NE]
                cls.EXPECT_NE(val1, val2, EXPECT_NE.lines[NE])
                NE += 1

            cls.TEST_SUMMARY(suite, test)

        cls.SUMMARY()
        # tearDown()


class EXPECT_EQ:
    values = []
    iters = []
    lines = []
    n = -1

    def __new__(cls, val1, val2):
        line = currentframe().f_back.f_lineno
        cls.lines.append(line)
        cls.values.append([val1, val2])
        cls.iters[cls.n] += 1

    @classmethod
    def itersAppend(cls):
        cls.iters.append(0)
        cls.n += 1


class EXPECT_NE:
    values = []
    iters = []
    lines = []
    n = -1

    def __new__(cls, val1, val2):
        line = currentframe().f_back.f_lineno
        cls.lines.append(line)
        cls.values.append([val1, val2])
        cls.iters[cls.n] += 1

    @classmethod
    def itersAppend(cls):
        cls.iters.append(0)
        cls.n += 1
