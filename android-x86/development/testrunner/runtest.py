#!/usr/bin/python2.4
#
# Copyright 2008, The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Command line utility for running a pre-defined test.

Based on previous <androidroot>/development/tools/runtest shell script.
"""

# Python imports
import glob
import optparse
import os
from sets import Set
import sys

# local imports
import adb_interface
import android_build
import coverage
import errors
import logger
import run_command
import test_defs


class TestRunner(object):
  """Command line utility class for running pre-defined Android test(s)."""

  _TEST_FILE_NAME = "test_defs.xml"

  # file path to android core platform tests, relative to android build root
  # TODO move these test data files to another directory
  _CORE_TEST_PATH = os.path.join("development", "testrunner", 
                                 _TEST_FILE_NAME)

  # vendor glob file path patterns to tests, relative to android
  # build root
  _VENDOR_TEST_PATH = os.path.join("vendor", "*", "tests", "testinfo",
                                   _TEST_FILE_NAME)

  _RUNTEST_USAGE = (
      "usage: runtest.py [options] short-test-name[s]\n\n"
      "The runtest script works in two ways.  You can query it "
      "for a list of tests, or you can launch one or more tests.")

  def __init__(self):
    # disable logging of timestamp
    logger.SetTimestampLogging(False)  

  def _ProcessOptions(self):
    """Processes command-line options."""
    # TODO error messages on once-only or mutually-exclusive options.
    user_test_default = os.path.join(os.environ.get("HOME"), ".android",
                                     self._TEST_FILE_NAME)

    parser = optparse.OptionParser(usage=self._RUNTEST_USAGE)

    parser.add_option("-l", "--list-tests", dest="only_list_tests",
                      default=False, action="store_true",
                      help="To view the list of tests")
    parser.add_option("-b", "--skip-build", dest="skip_build", default=False,
                      action="store_true", help="Skip build - just launch")
    parser.add_option("-n", "--skip_execute", dest="preview", default=False,
                      action="store_true",
                      help="Do not execute, just preview commands")
    parser.add_option("-r", "--raw-mode", dest="raw_mode", default=False,
                      action="store_true",
                      help="Raw mode (for output to other tools)")
    parser.add_option("-a", "--suite-assign", dest="suite_assign_mode",
                      default=False, action="store_true",
                      help="Suite assignment (for details & usage see "
                      "InstrumentationTestRunner)")
    parser.add_option("-v", "--verbose", dest="verbose", default=False,
                      action="store_true",
                      help="Increase verbosity of %s" % sys.argv[0])
    parser.add_option("-w", "--wait-for-debugger", dest="wait_for_debugger",
                      default=False, action="store_true",
                      help="Wait for debugger before launching tests")
    parser.add_option("-c", "--test-class", dest="test_class",
                      help="Restrict test to a specific class")
    parser.add_option("-m", "--test-method", dest="test_method",
                      help="Restrict test to a specific method")
    parser.add_option("-u", "--user-tests-file", dest="user_tests_file",
                      metavar="FILE", default=user_test_default,
                      help="Alternate source of user test definitions")
    parser.add_option("-o", "--coverage", dest="coverage",
                      default=False, action="store_true",
                      help="Generate code coverage metrics for test(s)")
    parser.add_option("-t", "--all-tests", dest="all_tests",
                      default=False, action="store_true",
                      help="Run all defined tests")
    parser.add_option("--continuous", dest="continuous_tests",
                      default=False, action="store_true",
                      help="Run all tests defined as part of the continuous "
                      "test set")

    group = optparse.OptionGroup(
        parser, "Targets", "Use these options to direct tests to a specific "
        "Android target")
    group.add_option("-e", "--emulator", dest="emulator", default=False,
                     action="store_true", help="use emulator")
    group.add_option("-d", "--device", dest="device", default=False,
                     action="store_true", help="use device")
    group.add_option("-s", "--serial", dest="serial",
                     help="use specific serial")
    parser.add_option_group(group)

    self._options, self._test_args = parser.parse_args()

    if (not self._options.only_list_tests and not self._options.all_tests
        and not self._options.continuous_tests and len(self._test_args) < 1):
      parser.print_help()
      logger.SilentLog("at least one test name must be specified")
      raise errors.AbortError

    self._adb = adb_interface.AdbInterface()
    if self._options.emulator:
      self._adb.SetEmulatorTarget()
    elif self._options.device:
      self._adb.SetDeviceTarget()
    elif self._options.serial is not None:
      self._adb.SetTargetSerial(self._options.serial)

    if self._options.verbose:
      logger.SetVerbose(True)

    self._root_path = android_build.GetTop()

    self._known_tests = self._ReadTests()

    self._coverage_gen = coverage.CoverageGenerator(
        android_root_path=self._root_path, adb_interface=self._adb)

  def _ReadTests(self):
    """Parses the set of test definition data.

    Returns:
      A TestDefinitions object that contains the set of parsed tests.
    Raises:
      AbortError: If a fatal error occurred when parsing the tests.
    """
    core_test_path = os.path.join(self._root_path, self._CORE_TEST_PATH)
    try:
      known_tests = test_defs.TestDefinitions()
      known_tests.Parse(core_test_path)
      # read all <android root>/vendor/*/tests/testinfo/test_defs.xml paths
      vendor_tests_pattern = os.path.join(self._root_path,
                                          self._VENDOR_TEST_PATH)
      test_file_paths = glob.glob(vendor_tests_pattern)
      for test_file_path in test_file_paths:
        known_tests.Parse(test_file_path)
      if os.path.isfile(self._options.user_tests_file):
        known_tests.Parse(self._options.user_tests_file)
      return known_tests
    except errors.ParseError:
      raise errors.AbortError

  def _DumpTests(self):
    """Prints out set of defined tests."""
    print "The following tests are currently defined:"
    for test in self._known_tests:
      print test.GetName()

  def _DoBuild(self):
    logger.SilentLog("Building tests...")
    target_set = Set()
    for test_suite in self._GetTestsToRun():
      self._AddBuildTarget(test_suite.GetBuildPath(), target_set)

    if target_set:
      if self._options.coverage:
        self._coverage_gen.EnableCoverageBuild()
        self._AddBuildTarget(self._coverage_gen.GetEmmaBuildPath(), target_set)
      target_build_string = " ".join(list(target_set))
      logger.Log("mmm %s" % target_build_string)
      cmd = 'ONE_SHOT_MAKEFILE="%s" make -C "%s" files' %  (target_build_string,
                                                            self._root_path)
      if self._options.preview:
        # in preview mode, just display to the user what command would have been
        # run
        logger.Log("adb sync")
      else:
        run_command.RunCommand(cmd, return_output=False)
        logger.Log("Syncing to device...")
        self._adb.Sync()

  def _AddBuildTarget(self, build_dir, target_set):
    if build_dir is not None:
      build_file_path = os.path.join(build_dir, "Android.mk")
      if os.path.isfile(os.path.join(self._root_path, build_file_path)):
        target_set.add(build_file_path)

  def _GetTestsToRun(self):
    """Get a list of TestSuite objects to run, based on command line args."""
    if self._options.all_tests:
      return self._known_tests.GetTests()
    if self._options.continuous_tests:
      return self._known_tests.GetContinuousTests()
    tests = []
    for name in self._test_args:
      test = self._known_tests.GetTest(name)
      if test is None:
        logger.Log("Error: Could not find test %s" % name)
        self._DumpTests()
        raise errors.AbortError
      tests.append(test)
    return tests

  def _RunTest(self, test_suite):
    """Run the provided test suite.

    Builds up an adb instrument command using provided input arguments.

    Args:
      test_suite: TestSuite to run
    """

    test_class = test_suite.GetClassName()
    if self._options.test_class is not None:
      test_class = self._options.test_class
    if self._options.test_method is not None:
      test_class = "%s#%s" % (test_class, self._options.test_method)

    instrumentation_args = {}
    if test_class is not None:
      instrumentation_args["class"] = test_class
    if self._options.wait_for_debugger:
      instrumentation_args["debug"] = "true"
    if self._options.suite_assign_mode:
      instrumentation_args["suiteAssignment"] = "true"
    if self._options.coverage:
      instrumentation_args["coverage"] = "true"
    if self._options.preview:
      adb_cmd = self._adb.PreviewInstrumentationCommand(
          package_name=test_suite.GetPackageName(),
          runner_name=test_suite.GetRunnerName(),
          raw_mode=self._options.raw_mode,
          instrumentation_args=instrumentation_args)
      logger.Log(adb_cmd)
    else:
      self._adb.StartInstrumentationNoResults(
          package_name=test_suite.GetPackageName(),
          runner_name=test_suite.GetRunnerName(),
          raw_mode=self._options.raw_mode,
          instrumentation_args=instrumentation_args)
      if self._options.coverage and test_suite.GetTargetName() is not None:
        coverage_file = self._coverage_gen.ExtractReport(test_suite)
        if coverage_file is not None:
          logger.Log("Coverage report generated at %s" % coverage_file)

  def RunTests(self):
    """Main entry method - executes the tests according to command line args."""
    try:
      run_command.SetAbortOnError()
      self._ProcessOptions()
      if self._options.only_list_tests:
        self._DumpTests()
        return

      if not self._adb.IsDevicePresent():
        logger.Log("Error: specified device cannot be found")
        return

      if not self._options.skip_build:
        self._DoBuild()

      for test_suite in self._GetTestsToRun():
        self._RunTest(test_suite)
    except KeyboardInterrupt:
      logger.Log("Exiting...")
    except errors.AbortError:
      logger.SilentLog("Exiting due to AbortError...")
    except errors.WaitForResponseTimedOutError:
      logger.Log("Timed out waiting for response")


def RunTests():
  runner = TestRunner()
  runner.RunTests()

if __name__ == "__main__":
  RunTests()
