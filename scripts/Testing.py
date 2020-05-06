from inspect import *
import datetime as dt


class TEST:
    failedTestsNames = []
    passedTests = 0
    failedTests = 0
    failed = False

    totalTime = 0
    suiteTime = 0

    def __new__(cls, *args):
        time = ((dt.datetime.now() - Suite.starttime).total_seconds() * 1000)
        Suite.times.append(time)
        cls.prevsuite = args[0].name
        cls.main(*args)

    @classmethod
    def TEST_SUMMARY(cls, suite, test, time):
        cls.suiteTime += time
        cls.totalTime += time

        if not cls.failed:
            print("[       OK ] {0}.{1} ({2} ms)".format(suite.name, test, round(time, 2)))
            cls.passedTests += 1
        else:
            print("[  FAILED  ] {0}.{1} ({2} ms)".format(suite.name, test, round(time, 2)))
            cls.failedTests += 1
            cls.failedTestsNames.append('{0}.{1}'.format(suite.name, test))

        if suite.name != cls.prevsuite:
            print("[----------] {0} tests from {1} ({2} ms total)".format(int(len(suite.tests)/2),
                                                                          suite.name, round(cls.suiteTime, 2)))
            cls.suiteTime = 0

        cls.prevsuite = suite
        cls.failed = False

    @classmethod
    def SUMMARY(cls, *args):
        print("[==========] {0} tests from {1} test suites ran. ({2} ms total)".format(cls.testsQuantity(*args),
                                                                                       cls.suitesQuantity(*args),
                                                                                       round(cls.totalTime, 2)))
        print("[  PASSED  ] {0} tests.".format(cls.passedTests))
        if cls.failedTests != 0:
            print("[  FAILED  ] {0} tests, listed below:".format(cls.failedTests))
        else:
            print("[  FAILED  ] {0} tests.".format(cls.failedTests))

        for name in cls.failedTestsNames:
            print("[  FAILED  ] {0}".format(name))

    @classmethod
    def testsQuantity(cls, *args):
        tests = 0
        for suite in args:
            tests += int(len(suite.tests)/2)
        return tests

    @classmethod
    def suitesQuantity(cls, *args):
        return len(args)

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
            frame = getframeinfo(currentframe().f_back.f_back.f_back)
            f = open(frame.filename)
            for i, c in enumerate(f):
                if i == line - 1:
                    content = c
            f.close()

            args = content[content.find('(') + 1:-2].split(', ')

            # Print error message
            print("{0}:{1}: Failure".format(frame.filename, line))
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
            frame = getframeinfo(currentframe().f_back.f_back.f_back)
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
    def main(cls, *args):
        # setUp()
        print("[==========] Running {0} tests from {1} test suites".format(cls.testsQuantity(*args),
                                                                           cls.suitesQuantity(*args)))
        t = 0

        for suite in args:
            EQiter = iter(suite.equal)
            NEiter = iter(suite.notequal)
            for i in range(int(len(suite.tests)/2)):
                test = suite.tests[2*i]
                E, N = suite.tests[(2*i)+1]
                print("[ RUN      ] {0}.{1}".format(suite.name, test))

                for pair in range(E):
                    val1, val2, line = next(EQiter)
                    cls.EXPECT_EQ(val1, val2, line)

                for pair in range(N):
                    val1, val2, line = next(NEiter)
                    cls.EXPECT_NE(val1, val2, line)

                cls.TEST_SUMMARY(suite, test, Suite.times[t])
                t += 1

        cls.SUMMARY(*args)
        # tearDown()


class Suite:
    times = []
    starttime = 0

    def __init__(self, name):
        self.name = name
        self.tests = []
        self.equal = []
        self.notequal = []

    def addTest(self, test):
        if Suite.starttime != 0:
            time = ((dt.datetime.now() - Suite.starttime).total_seconds() * 1000)
            Suite.times.append(time)
        Suite.starttime = dt.datetime.now()
        self.tests.append(test)
        self.tests.append([0, 0])
        self.test = test

    def EQ(self, val1, val2):
        idx = self.tests.index(self.test)
        self.tests[idx+1][0] += 1
        line = currentframe().f_back.f_lineno
        self.equal.append([val1, val2, line])

    def NE(self, val1, val2):
        idx = self.tests.index(self.test)
        self.tests[idx+1][1] += 1
        line = currentframe().f_back.f_lineno
        self.notequal.append([val1, val2, line])
