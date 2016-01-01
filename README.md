# isri-ocr-evaluation-tools

[![Build Status](https://travis-ci.org/eddieantonio/isri-ocr-evaluation-tools.svg?branch=master)](https://travis-ci.org/eddieantonio/isri-ocr-evaluation-tools)

The ISRI Analytic Tools consist of 17 tools for measuring the
performance of and experimenting with OCR output. See [the user
guide][user-guide] for more information.

[user-guide]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/blob/HEAD/user-guide.pdf

# Install (OS X)

Using [Homebrew][brew]:

    brew tap eddieantonio/eddieantonio
    brew install isri-ocr-evaluation-tools

[brew]: http://brew.sh/

Building
========

Each subdirectory contains the source for one program, a Unix man
page, and a Makefile.  Most programs depend on a library in Modules/
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

The automated way (copy-paste this one line):

    curl -fsSL http://www.eddieantonio.ca/isri-ocr-evaluation-tools/install_utf8proc.sh | sh

The manual way:

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

## Installing

Install to `/usr/local/`:

    make install

# Porting Credits

Ported by Eddie Antonio Santos, 2015, 2016. See analytical-tools/NOTICE
for copyright information regarding the original code.

# License

The ISRI Analytic Tools for OCR Evaluation

Copyright 2015, 2016 Eddie Antonio Santos

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
