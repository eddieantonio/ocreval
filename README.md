# ocreval

[![Build Status](https://travis-ci.org/eddieantonio/orceval.svg?branch=master)](https://travis-ci.org/eddieantonio/ocreval)

The `ocreval` consist of 17 tools for measuring the
performance of and experimenting with OCR output. See [the user
guide][user-guide] for more information.

[user-guide]: https://github.com/eddieantonio/ocreval/raw/master/user-guide.pdf

`ocreval` is a modern port of the [ISRI Analytic Tools][], with UTF-8
support and other improvements.

[ISRI Analytic Tools]: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.216.9427&rep=rep1&type=pdf

# Install (macOS)

Using [Homebrew][brew]:

    brew tap eddieantonio/eddieantonio
    brew install ocreval

[brew]: http://brew.sh/


Building
========

To build the library and all of the programs, simply type `make`.

## Dependencies

 - [utf8proc](https://github.com/JuliaLang/utf8proc)

### OS X

    brew install utf8proc

### Ubuntu/Debian

If this package is not in your package repository, follow "Other Linux"
below.

    sudo apt-get install libutf8proc-dev

### Other Linux

Install `libutf8proc-dev` manually:

    curl -OL https://github.com/JuliaLang/utf8proc/archive/v1.3.1.tar.gz
    tar xzf v1.3.1.tar.gz
    cd utf8proc-1.3.1/
    make
    sudo make install
    # Rebuild the shared object cache - needed to load the library
    # at runtime <http://linux.die.net/man/8/ldconfig>
    sudo ldconfig
    cd -

## Building the tools

    make

## Installing globally

Install to `/usr/local/`:

    sudo make install

Note: You will not need `sudo` on OS X if you have `brew` installed.

## Installing "locally"

This will not copy any files at all, but instead create the appropriate
shell commands to add all executables, man pages, and libraries to
the correct path (replace `~/.bashrc` with your start-up file):

    make exports >> ~/.bashrc

# Porting Credits

Ported by Eddie Antonio Santos, 2015, 2016. See analytical-tools/NOTICE
for copyright information regarding the original code.

# License

### ocreval

Copyright 2015–2018 Eddie Antonio Santos

### The ISRI Analytic Tools for OCR Evaluation

Copyright 1996 The Board of Regents of the Nevada System of Higher
Education, on behalf, of the University of Nevada, Las Vegas,
Information Science Research Institute

Licensed under the Apache License, Version 2.0 (the "License"); you
may not use this file except in compliance with the License.  You may
obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing
permissions and limitations under the License.
