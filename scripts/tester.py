#!/usr/bin/env python
#
# Copyright (C) 2016 Ben Smith
#
# This software may be modified and distributed under the terms
# of the MIT license.  See the LICENSE file for details.
#
from __future__ import print_function
import argparse
import collections
import json
import multiprocessing
import os
import sys
import time

import common

TEST_JSON = os.path.join(common.SCRIPT_DIR, 'test.json')
TEST_RESULT_DIR = os.path.join(common.OUT_DIR, 'test_results')
TEST_RESULTS_MD = os.path.join(common.ROOT_DIR, 'test_results.md')

OK      = '[OK] '
FAIL    = '[X]  '
UNKNOWN = '[?]  '

Test = collections.namedtuple('Test', ['json', 'suite', 'index', 'rom', 'frames', 'hash', 'joyp'])
TestResult = collections.namedtuple('TestResult',
                                    ['test', 'passed', 'ok', 'message', 'duration'])


def RunTest(test, options):
  start_time = time.time()
  png = os.path.relpath(os.path.splitext(test.rom)[0] + '.png', common.ROOT_DIR)
  png = os.path.join(TEST_RESULT_DIR, png.replace(os.path.sep, '_'))
  try:
    common.RunTester(test.rom, test.frames, png, controller_input=test.joyp,
                     exe=options.exe)
    actual = common.HashFile(png)

    if test.hash.startswith('!'):
      expect_fail = True
      expected = test.hash[1:]
    else:
      expect_fail = False
      expected = test.hash

    message = ''
    ok = actual == expected
    if ok:
      if expect_fail and options.verbose > 0:
        message = FAIL + test.rom
      elif options.verbose > 1:
        message = OK + test.rom
    else:
      if expected == '' or expect_fail:
        message = UNKNOWN + '%s => %s' % (test.rom, actual)
      else:
        message = FAIL + '%s => %s' % (test.rom, actual)

    passed = ok and not expect_fail
    if not passed and options.rebase:
      # print(test.rom, test.json[2], actual)
      if expect_fail:
        test.json[2] = '!' + actual
      else:
        test.json[2] = actual
    duration = time.time() - start_time
    return TestResult(test, passed, ok, message, duration)
  except (common.Error, KeyboardInterrupt) as e:
    duration = time.time() - start_time
    message = FAIL + '%s => %s' % (test.rom, str(e))
    return TestResult(test, False, False, message, duration)


last_message_len = 0
def PrintReplace(s, newline=False):
  global last_message_len
  if sys.stdout.isatty():
    sys.stdout.write('\r' + s)
    if len(s) < last_message_len:
      sys.stdout.write(' ' * (last_message_len - len(s)))
    if newline:
      sys.stdout.write('\n')
    else:
      sys.stdout.flush()
    last_message_len = len(s)
  else:
    sys.stdout.write(s)
    sys.stdout.write('\n')


def RunAllTests(tests, options):
  results = []
  pool = multiprocessing.Pool(options.num_processes)
  try:
    tasks = [pool.apply_async(RunTest, (test, options)) for test in tests]
    passed = 0
    failed = 0
    while tasks:
      new_tasks = []
      for task in tasks:
        if task.ready():
          result = task.get(0)
          results.append(result)

          if result.ok:
            passed += 1
          else:
            failed += 1

          if result.message:
            PrintReplace(result.message, newline=True)

          PrintReplace('[+%d|-%d|%%%d] %s (%.3fs)' % (
            passed, failed, 100 * ((passed + failed) / len(tests)),
            result.test.rom, result.duration))
        else:
          new_tasks.append(task)
      time.sleep(0.01)
      tasks = new_tasks
    pool.close()
  except KeyboardInterrupt:
    pass
  finally:
    pool.terminate()
    pool.join()

  return results


def MDLink(text, url):
  return '[%s](%s)' % (text, url)

def MDTableRow(*columns):
  return '| %s |\n' % ' | '.join(columns)

def MDTestName(rom, prefix):
  assert rom.startswith(prefix)
  name = rom[len(prefix):]
  name = os.path.splitext(name)[0]
  name = name.replace('_', '\\_')
  return name

def MDTestResult(passed):
  return ':ok:' if passed else ':x:'

def SuiteHeader(out_file, name):
  out_file.write('%s:\n\n' % name)
  out_file.write(MDTableRow('Test', 'Result'))
  out_file.write(MDTableRow('---', '---'))

def Suite(out_file, results, suite, prefix):
  for result in sorted(results, key=lambda x: x.test.rom):
    if result.test.suite != suite:
      continue
    name = MDTestName(result.test.rom, prefix)
    out_file.write(MDTableRow(name, MDTestResult(result.passed)))
  out_file.write('\n')

def GenerateTestResults(results):
  SUITES = [
    {'name': 'apu_test', 'prefix': 'nes-test-roms/apu_test/rom_singles/'},
    {'name': 'blargg_apu', 'prefix': 'nes-test-roms/blargg_apu_2005.07.30/'},
    {'name': 'blargg_nes_cpu_test5', 'prefix': 'nes-test-roms/blargg_nes_cpu_test5/'},
    {'name': 'blargg_ppu_tests', 'prefix': 'nes-test-roms/blargg_ppu_tests_2005.09.15b/'},
    {'name': 'branch_timing_tests', 'prefix': 'nes-test-roms/branch_timing_tests/'},
    {'name': 'cpu_dummy_reads', 'prefix': 'nes-test-roms/cpu_dummy_reads/'},
    {'name': 'cpu_dummy_writes', 'prefix': 'nes-test-roms/cpu_dummy_writes/'},
    {'name': 'cpu_exec_space', 'prefix': 'nes-test-roms/cpu_exec_space/'},
    {'name': 'cpu_interrupts_v2', 'prefix': 'nes-test-roms/cpu_interrupts_v2/rom_singles/'},
    {'name': 'cpu_timing_test6', 'prefix': 'nes-test-roms/cpu_timing_test6/'},
    {'name': 'dmc_dma_during_read4', 'prefix': 'nes-test-roms/dmc_dma_during_read4/'},
    {'name': 'dmc_tests', 'prefix': 'nes-test-roms/dmc_tests/'},
    {'name': 'instr_misc', 'prefix': 'nes-test-roms/instr_misc/rom_singles/'},
    {'name': 'instr_test-v5', 'prefix': 'nes-test-roms/instr_test-v5/rom_singles/'},
    {'name': 'instr_timing', 'prefix': 'nes-test-roms/instr_timing/rom_singles/'},
    {'name': 'mmc3_test', 'prefix': 'nes-test-roms/mmc3_test_2/rom_singles/'},
    {'name': 'nes_instr_test', 'prefix': 'nes-test-roms/nes_instr_test/rom_singles/'},
    {'name': 'oam_read', 'prefix': 'nes-test-roms/oam_read/'},
    {'name': 'oam_stress', 'prefix': 'nes-test-roms/oam_stress/'},
    {'name': 'ppu_open_bus', 'prefix': 'nes-test-roms/ppu_open_bus/'},
    {'name': 'ppu_read_buffer', 'prefix': 'nes-test-roms/ppu_read_buffer/'},
    {'name': 'ppu_vbl_nmi', 'prefix': 'nes-test-roms/ppu_vbl_nmi/rom_singles/'},
    {'name': 'sprdma_and_dmc_dma', 'prefix': 'nes-test-roms/sprdma_and_dmc_dma/'},
    {'name': 'sprite_hit_tests', 'prefix': 'nes-test-roms/sprite_hit_tests_2005.10.05/'},
    {'name': 'sprite_overflow_tests', 'prefix': 'nes-test-roms/sprite_overflow_tests/'},
    {'name': 'vbl_nmi_timing', 'prefix': 'nes-test-roms/vbl_nmi_timing/'},
  ]

  with open(TEST_RESULTS_MD, 'w') as out_file:
    out_file.write('# Test status\n\n')
    for suite in SUITES:
      SuiteHeader(out_file, suite['name'])
      Suite(out_file, results, suite['name'], suite['prefix'])


def GetPath(dir_, p):
  if p.startswith('//'): return os.path.join(common.ROOT_DIR, p[2:])
  return os.path.join(dir_, p)

def main(args):
  parser = argparse.ArgumentParser()
  parser.add_argument('patterns', metavar='pattern', nargs='*',
                      help='test patterns.')
  parser.add_argument('-f', '--file', default=TEST_JSON)
  parser.add_argument('-j', '--num-processes',
                      type=int, default=multiprocessing.cpu_count(),
                      help='num processes.')
  parser.add_argument('-e', '--exe', help='path to tester')
  parser.add_argument('-v', '--verbose', action='count', default=0,
                      help='show more info')
  parser.add_argument('-g', '--generate', action='store_true',
                      help='generate test result markdown')
  parser.add_argument('-r', '--rebase', action='store_true',
                      help='reset all failing tests to the new hash values')
  options = parser.parse_args(args)
  pattern_re = common.MakePatternRE(options.patterns)
  passed = 0
  if not os.path.exists(TEST_RESULT_DIR):
    os.makedirs(TEST_RESULT_DIR)

  tests = []
  test_json = json.load(open(options.file))
  for suite_name, suite in test_json.items():
    dir_ = suite['dir']
    for index, test in enumerate(suite['tests']):
      joyp = GetPath(dir_, test[3]) if len(test) == 4 else None
      tests.append(Test(test, suite_name, index, GetPath(dir_, test[0]), test[1], test[2], joyp))

  tests = [test for test in tests if pattern_re.match(test.rom)]

  start_time = time.time()
  results = RunAllTests(tests, options)
  duration = time.time() - start_time
  PrintReplace('total time: %.3fs' % duration, newline=True)

  if options.rebase:
    for result in results:
      test = result.test
      test_json[test.suite]["tests"][test.index] = test.json
    with open(options.file, 'wt') as outfile:
      # Write json with json.dump so it formats it the way I like.
      outfile.write('{\n')
      for i, (suite_name, suite) in enumerate(test_json.items()):
        outfile.write(f'  "{suite_name}": {{\n')
        outfile.write(f'    "dir": "{suite["dir"]}",\n')
        outfile.write(f'    "tests": [\n')
        for j, test in enumerate(suite['tests']):
          outfile.write(f'      ["{test[0]}", {test[1]}, "{test[2]}"')
          if len(test) == 4:
            outfile.write(f', "{test[3]}"')
          outfile.write(f']')
          if j != len(suite['tests']) - 1:
            outfile.write(',')
          outfile.write('\n')
        outfile.write('    ]\n')
        outfile.write('  }')
        if i != len(test_json) - 1:
          outfile.write(',')
        outfile.write('\n')
      outfile.write('}\n')
      # json.dump(test_json, outfile, indent=2)
    pass

  ok = sum(1 for result in results if result.ok)
  passed = sum(1 for result in results if result.passed)
  completed = len(results)
  print('Passed %d/%d' % (passed, completed))

  if options.generate:
    GenerateTestResults(results)

  return 0 if ok == completed else 1

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
