#!/usr/bin/python2.4
#
#
# Copyright 2007, The Android Open Source Project
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

# System imports
import os
import signal
import subprocess
import time
import threading

# local imports
import logger
import errors

_abort_on_error = False

def SetAbortOnError(abort=True):
  """Sets behavior of RunCommand to throw AbortError if command process returns 
  a negative error code"""
  global _abort_on_error
  _abort_on_error = abort
  
def RunCommand(cmd, timeout_time=None, retry_count=3, return_output=True):
  """Spawns a subprocess to run the given shell command, and checks for
  timeout_time. If return_output is True, the output of the command is returned
  as a string. Otherwise, output of command directed to stdout """

  result = None
  while True:
    try:
      result = RunOnce(cmd, timeout_time=timeout_time, 
                       return_output=return_output)
    except errors.WaitForResponseTimedOutError:
      if retry_count == 0:
        raise
      retry_count -= 1
      logger.Log("No response for %s, retrying" % cmd)
    else:
      # Success
      return result

def RunOnce(cmd, timeout_time=None, return_output=True):
  start_time = time.time()
  so = []
  pid = []
  global _abort_on_error
  error_occurred = False
  
  def Run():
    if return_output:
      output_dest = subprocess.PIPE
    else:
      # None means direct to stdout
      output_dest = None  
    pipe = subprocess.Popen(
        cmd,
        executable='/bin/bash',
        stdout=output_dest,
        stderr=subprocess.STDOUT,
        shell=True)
    pid.append(pipe.pid)
    try:
      output = pipe.communicate()[0]
      if output is not None and len(output) > 0:
        so.append(output)
    except OSError, e:
      logger.SilentLog("failed to retrieve stdout from: %s" % cmd)
      logger.Log(e)
      so.append("ERROR")
      error_occurred = True
    if pipe.returncode < 0:
      logger.SilentLog("Error: %s was terminated by signal %d" %(cmd, 
          pipe.returncode))
      error_occurred = True  

  t = threading.Thread(target=Run)
  t.start()

  break_loop = False
  while not break_loop:
    if not t.isAlive():
      break_loop = True

    # Check the timeout
    if (not break_loop and timeout_time is not None
        and time.time() > start_time + timeout_time):
      try:
        os.kill(pid[0], signal.SIGKILL)
      except OSError:
        # process already dead. No action required.
        pass

      logger.SilentLog("about to raise a timeout for: %s" % cmd)
      raise errors.WaitForResponseTimedOutError
    if not break_loop:
      time.sleep(0.1)

  t.join()

  if _abort_on_error and error_occurred:
    raise errors.AbortError
  
  return "".join(so)
