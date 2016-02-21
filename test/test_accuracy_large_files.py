#!/usr/bin/env python
# -*- encoding: UTF-8 -*-

"""
Tests accuracy on LARGE UTF-8 files.
"""

import random
import subprocess
import tempfile

import os.path as p


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

# Path to accuracy program
ACCURACY = p.join(p.dirname(p.dirname(p.realpath(__file__))),
                  'bin', 'accuracy')
assert p.exists(ACCURACY), 'Could not find ' + ACCURACY


# http://www.languagegeek.com/isolate/haidastory.html
corpus = u('''\
Aaniisuu tangaa g̱aging.ang ’wan suuga. ’Ll xidgwangaas, x̱uyaa’aa. Tllgu
ḵ’aawgyaa hllng.aaygi ’laa ḵyaang.aas. Ḵawdiuu gwaay g̱ud gwaa nang ḵadlaa
ḵ’ayg̱udyaas ’laagu ḵ’aawgaay g̱an ’laa g̱á ’laa xidaas. Á tl’l sg̱aana ḵidaads
’yaahlgaagaas g̱iinuus gangaang ’laagu gud gwii x̱iihlt’ahliyaagaas. Ga
sg̱aanag̱waa g̱ax̱aas ’laa t’isda ḵ’a sḵ’agilaang.aas, tll gwii x̱an, hahl gwii’ad
wah gwii’aa. G̱adagaas gyaanuu’asing g̱aalgaagaang ’wan suuga.

Nang kilsdlaas naag̱ag̱a.aw tadll chi’a’aawaagan. Sing ḵ’alg̱ada ’ll ḵaaxuhls
gyaan ’ll kindagaang.aas. Sda ’laa xid k’udahldats’aasii gyaan gagu ’laa
ḵ’aw’aawaasgu x̱an ’laa ḵ’aawgangas.
''')

dictionary = tuple(word for word in corpus.split())
alphabet = [char for char in corpus if char not in ' \n']


def one_in(n):
    return random.choice(xrange(n)) == 1


def change_letter(word):
    letter_index = random.choice(xrange(len(word)))
    mutation = random.choice(alphabet)
    return word[:letter_index] + mutation + word[letter_index + 1:]


if __name__ == '__main__':
    import sys
    amount_of_words = int(sys.argv[1]) if len(sys.argv) > 1 else 32768

    # Create temporary files for each...
    with tempfile.NamedTemporaryFile('wb') as correct_file,\
            tempfile.NamedTemporaryFile('wb') as generated_file:

        # Generate A LOT of random words
        for _ in xrange(amount_of_words):
            end = b'\n' if one_in(10) else b' '

            word = random.choice(dictionary)
            correct_file.write(word.encode('UTF-8'))

            # Occasionally, typo a word in the generated file.
            generated_word = change_letter(word) if one_in(1000) else word
            generated_file.write(generated_word.encode('UTF-8'))

            # Write a space or newline.
            correct_file.write(end)
            generated_file.write(end)

        # Finish off the file with a new line and flush the output.
        if end != b'\n':
            correct_file.write(b'\n')
            generated_file.write(b'\n')

        correct_file.flush()
        generated_file.flush()

        # This will fail if accuracy itself fails.
        subprocess.check_call([ACCURACY,
                               correct_file.name, generated_file.name])
