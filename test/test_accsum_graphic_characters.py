#!/usr/bin/env python
# -*- encoding: UTF-8 -*-
# Copyright 2017 Eddie Antonio Santos <easantos@ualberta.ca>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


"""
Tests accsum on UTF-8 files.
"""

import glob
import re
import shutil
import subprocess
import tempfile
import unicodedata

import os.path as p
from collections import namedtuple, OrderedDict

# Alias range as xrange in Python 3:
try:
    xrange
except NameError:
    xrange = range

# Create a Python 2/3 Unicode string literal:
try:
    unicode
except NameError:
    u = str
else:
    u = lambda s: s.decode('UTF-8')


# pathlib doesn't exist in Python 2, so make a small version of it:
class Path(str):
    def __div__(self, other):
        return Path(p.join(self, other))

    def exists(self):
        return p.exists(self)

    def create_file(self, filename, contents):
        with open(self / filename, 'w') as fp:
            fp.write(contents.encode('UTF-8'))
            if not contents.endswith('\n'):
                fp.write(b'\x0A')


# Path to accuracy program
BIN_DIR = Path(p.dirname(p.dirname(p.realpath(__file__)))) / 'bin'
ACCURACY_BIN = BIN_DIR / 'accuracy'
ACCSUM_BIN = BIN_DIR / 'accsum'
assert ACCURACY_BIN.exists(), 'Could not find ' + ACCURACY_BIN
assert ACCSUM_BIN.exists(), 'Could not find ' + ACCSUM_BIN


class FilePair(namedtuple('FilePairBase', 'correct generated')):
    """
    Pair of tests that are written as documents. Then an accuracy report may
    be produced.
    """

    @property
    def prefix(self):
        return str(hash(self.correct)).replace('-', '_')

    def write_to_dir(self, directory):
        directory.create_file(self.correct_filename, self.correct)
        directory.create_file(self.generated_filename, self.generated)

    @property
    def correct_filename(self):
        return '%s_correct' % self.prefix

    @property
    def generated_filename(self):
        return '%s_generated' % self.prefix

    @property
    def report_filename(self):
        return '%s_report' % self.prefix

    def write_accuracy_report(self, directory):
        self.write_to_dir(directory)

        # Return name of the report?
        subprocess.check_call([
            ACCURACY_BIN,
            directory / self.correct_filename,
            directory / self.generated_filename,
            directory / self.report_filename
        ])


class ClassResult(namedtuple('ResultBase', 'count missed right character')):
    pass


def extract_bracketed_char(text):
    match = re.match('^{(.+)}$', text)
    return match.group(1)


def nfc(text):
    """
    Returns NFC normalized text.
    """
    return unicodedata.normalize('NFC', u(text))


def nfd(text):
    """
    Returns NFD normalized text.
    """
    return unicodedata.normalize('NFD', u(text))


class ClassReport(object):
    """
    Wraps an accuracy report.
    """
    def __init__(self, *results):
        self._results = OrderedDict((
            (result.character, result) for result in results
        ))

    def __getitem__(self, key):
        return self._results[key]

    def __iter__(self):
        return iter(self._results)

    def __contains__(self, key):
        return key in self._results

    @classmethod
    def from_accuracy_report(cls, report_text):
        lines = report_text.split('\n\n')[-1].rstrip('\n').split('\n')

        # Assert we've got the right header
        count, missed, right = lines.pop(0).split()
        assert count == u('Count')
        assert missed == u('Missed')
        assert right == u('%Right')

        def generate_results():
            for line in lines:
                count, missed, right, char = line.lstrip().split(None, 3)
                char = extract_bracketed_char(char)
                yield ClassResult(int(count), int(missed), right, char)

        return cls(*list(generate_results()))


class TemporaryDirectory(object):
    """
    Context manager: creates a temporary directory and removes it when
    finished.
    """
    def __enter__(self):
        self._name = Path(tempfile.mkdtemp())
        return self._name

    def __exit__(self, *exc_info):
        shutil.rmtree(self._name)


def accsum(reports):
    """
    Runs accsum, returning a ClassReport (the final section in the report).
    """
    report_bytes = subprocess.check_output(
        [ACCSUM_BIN] + reports,
        stderr=subprocess.STDOUT
    )
    contents = report_bytes.decode('UTF-8')

    return ClassReport.from_accuracy_report(contents)


tests = [
    # Test some delimiting and special characters
    FilePair(correct=  nfc("{{"),
             generated=nfc("{<")),
    FilePair(correct=  nfc("<<"),
             generated=nfc("<{")),
    FilePair(correct=  nfc("q\\z"),
             generated=nfc("q|z")),

    # Latin scripts
    FilePair(correct=  nfc("Mirosław"),
             generated=nfc("Miroslaw")),
    # From: https://fi.wikipedia.org/w/index.php?title=Tekstintunnistus&oldid=15178566
    FilePair(correct=  nfc("""käsin kirjoittamalla"""),
             generated=nfc("""kasin kirjoittämalla""")),
    FilePair(correct=  nfc("""sähköisesti muokattavaan muotoon"""),
             generated=nfc("""sähköisesti muökattavaan muotoon""")),

    # Combining characters. Notice the use of NFD (decomposed)
    FilePair(correct  =nfd("q̃◌q̃"),
             generated=nfd("q̃◌q̂")),

    # Hiragana
    FilePair(correct=  nfc("""びょおいん"""),
             generated=nfc("""びよおいん""")),

    # Emoji
    FilePair(correct=  nfc("""💩"""),
             generated=nfc("""👜""")),
]

# TODO: Change this for an ACTUAL expected report (this one is incomplete)
expected_report = ClassReport.from_accuracy_report(u(
r"""ocreval Accuracy Report Version 7.0
-----------------------------------

   Count   Missed   %Right
       9        0   100.00   {<\n>}
       3        0   100.00   { }
       2        1    50.00   {<}
       2        1    50.00   {{}
       1        1    50.00   {\}
       1        0   100.00   {M}
       8        1    80.00   {a}
       1        0    80.00   {e}
       1        0   100.00   {h}
       6        0   100.00   {i}
       1        0   100.00   {j}
       4        0   100.00   {k}
       2        0   100.00   {l}
       3        0   100.00   {m}
       3        0   100.00   {n}
       6        1    83.33   {o}
       3        0   100.00   {q}
       2        0   100.00   {r}
       5        0   100.00   {s}
       6        0   100.00   {t}
       2        0   100.00   {u}
       1        0   100.00   {v}
       1        0   100.00   {w}
       1        0   100.00   {z}
       1        1     0.00   {ł}
       2        1    50.00   {ä}
       1        0   100.00   {ö}
       1        0   100.00   {び}
       1        1     0.00   {ょ}
       1        0   100.00   {お}
       1        0   100.00   {い}
       1        0   100.00   {ん}
       1        1     0.00   {💩}
       2        1    50.00   {◌̃}
       1        0   100.00   {◌}
"""))


def main(temp_dir):
    # Create each individual accuracy report:
    for test in tests:
        test.write_accuracy_report(temp_dir)

    reports = glob.glob(temp_dir / '*_report')
    assert len(reports) == len(tests)

    # Create the accuracy summary!
    actual_report = accsum(reports)

    for char in expected_report:
        # Check if the character is even in the report.
        assert char in actual_report, (
            '{%s} not in report: %r' % (char, set(actual_report))
        )

        # Check that the counts match
        expected, actual = expected_report[char], actual_report[char]
        assert expected.count == actual.count, (
            '{%s}: counts does not match expected: %d; actual: %d' % (
                char, expected.count, actual.count
            )
        )
        assert expected.missed == actual.missed, (
            '{%s}: #missed does not match expected: %d; actual: %d' % (
                char, expected.missed, actual.missed
            )
        )

    difference = set(actual_report) - set(expected_report)
    assert len(difference) == 0, (
        'Actual report has extra characters: %r' % (difference,)
    )


if __name__ == '__main__':
    import sys
    try:
        _, flag = sys.argv
    except:
        debug = False
    else:
        debug = flag == '--debug'

    # Create temporary files for each...
    with TemporaryDirectory() as temp_dir:
        try:
            main(temp_dir)
        except subprocess.CalledProcessError as error:
            sys.stderr.write("Error %d running command: %s" % (
                error.returncode,
                ' '.join(error.cmd)
            ))
            sys.stderr.write("\n")

            if error.output is not None:
                sys.stderr.write("\n--- stdout ---\n")
                sys.stderr.write(error.output)

            if debug:
                import pdb
                pdb.set_trace()
            sys.exit(-1)
        except AssertionError as error:
            if debug:
                import pdb
                pdb.set_trace()
            print(error.message)
            sys.exit(-1)
