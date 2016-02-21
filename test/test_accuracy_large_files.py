#!/usr/bin/env python
# -*- encoding: UTF-8 -*-

from __future__ import print_function

"""
Tests accuracy on LARGE files.
"""

import random
import string
import subprocess
import tempfile

import os.path as p


try:
    xrange
except NameError:
    xrange = range


# Path to accuracy program
ACCURACY = p.join(p.dirname(p.dirname(p.realpath(__file__))),
                        'bin', 'accuracy')
assert p.exists(ACCURACY), 'Could not find ' + ACCURACY


def one_in(n):
    return random.choice(xrange(n)) == 1


def change_letter(word):
    letter_index = random.choice(xrange(len(word)))
    mutation = random.choice(string.ascii_lowercase)
    return word[:letter_index] + mutation + word[letter_index + 1:]


if __name__ == '__main__':
    import sys
    amount_of_words = int(sys.argv[1]) if len(sys.argv) > 1 else 32768

    # Load up a bunch of real words.
    with open('/usr/share/dict/words', 'r') as word_file:
        dictionary = [line.strip().lower() for line in word_file]

    # Create temporary files for each...
    with tempfile.NamedTemporaryFile('w') as correct_file,\
            tempfile.NamedTemporaryFile('w') as generated_file:

        # Generate A LOT of random words
        for _ in xrange(amount_of_words):
            end = '\n' if one_in(10) else ' '

            word = random.choice(dictionary)
            print(word, end=end, file=correct_file)

            # Occasionally, typo a word in the generated file.
            generated_word = change_letter(word) if one_in(1000) else word
            print(generated_word, end=end, file=generated_file)

        # Finish off the file with a new line and flush the output.
        print(file=correct_file)
        print(file=generated_file)
        correct_file.flush()
        generated_file.flush()

        # This will fail if accuracy itself fails.
        subprocess.check_call([ACCURACY,
                               correct_file.name, generated_file.name])
