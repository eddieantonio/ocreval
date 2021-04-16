# ocreval

[![Build Status](https://travis-ci.org/eddieantonio/ocreval.svg?branch=master)](https://travis-ci.org/eddieantonio/ocreval)

The `ocreval` consist of 17 tools for measuring the
performance of and experimenting with OCR output. See [the user
guide][user-guide] for more information.

[user-guide]: https://github.com/eddieantonio/ocreval/raw/master/user-guide.pdf

`ocreval` is a modern port of the [ISRI Analytic Tools for OCR Evaluation][isri],
with UTF-8 support and other improvements.

See [the archived Google Code repository of the original
project][isri-code]!

[isri]: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.216.9427&rep=rep1&type=pdf
[isri-code]: http://code.google.com/p/isri-ocr-evaluation-tools

# Install (macOS)

Using [Homebrew][brew]:

    brew install eddieantonio/eddieantonio/ocreval

[brew]: http://brew.sh/


Building
========

To build the library and all of the programs, ensure that you have all
required [dependencies](#dependencies).

## Dependencies

`ocreval` requires [utf8proc](https://github.com/JuliaStrings/utf8proc)
to build from source.

### macOS

Using [Homebrew][brew]:

    brew install utf8proc

### Ubuntu/Debian

You may need to install `make` and a C compiler:

    sudo apt install build-essential

Then install, `libutf8proc-dev`:

    sudo apt install libutf8proc-dev

If `libutf8proc-dev` cannot be installed using `apt`, follow
[Other Linux](#other-linux) below

### Other Linux

Install `libutf8proc-dev` manually:

    curl -OL https://github.com/JuliaStrings/utf8proc/archive/v1.3.1.tar.gz
    tar xzf v1.3.1.tar.gz
    cd utf8proc-1.3.1/
    make
    sudo make install
    # Rebuild the shared object cache - needed to load the library
    # at runtime <http://linux.die.net/man/8/ldconfig>
    sudo ldconfig
    cd -

## Building the tools

Once all dependencies are installed, you may compile all of the
utilities using `make`:

    make

## Installing

Install to `/usr/local/`:

    sudo make install

Note: You will not need `sudo` on macOS if you have `brew` installed.

## Installing "locally"

This will not copy any files at all, but instead create the appropriate
shell commands to add all executables, man pages, and libraries to
the correct path (replace `~/.bashrc` with your start-up file):

    make exports >> ~/.bashrc

# Porting Credits

Ported by Eddie Antonio Santos, 2015, 2016. See `NOTICE` for copyright
information regarding the original code.

# Citation

```bibtex
@inproceedings{santos-2019-ocr,
    title = "{OCR} evaluation tools for the 21st century",
    author = "Santos, Eddie Antonio",
    booktitle = "Proceedings of the 3rd Workshop on the Use of Computational Methods in the Study of Endangered Languages Volume 1 (Papers)",
    month = feb,
    year = "2019",
    address = "Honolulu",
    publisher = "Association for Computational Linguistics",
    url = "https://www.aclweb.org/anthology/W19-6004",
    pages = "23--27",
}
```

See: <https://www.aclweb.org/anthology/W19-6004/>


# License

### ocreval

Copyright 2015–2017 Eddie Antonio Santos

Copyright © 2018–2021 National Research Council Canada

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
