# isri-ocr-evaluation-tools

The ISRI Analytic Tools for OCR Evaluation

Ported to compile on modern systems (like OS X, El Capitan).

# Install (OS X)

Using [Homebrew][brew]:

    brew tap eddieantonio/eddieantonio
    brew install isri-ocr-evaluation-tools

[brew]: http://brew.sh/

# Building

## Dependencies

 - [utf8proc](https://github.com/JuliaLang/utf8proc)

        # OS X
        brew tap eddieantonio/eddieantonio
        brew install utf8proc

        # Ubuntu
        sudo apt-get install libutf8proc-dev

# Building the tools

Either:

    cd analytical-tools && make

Or:

    make -C analytical-tools


# Porting Credits

Ported by Eddie Antonio Santos, 2015. See analytical-tools/NOTICE for
copyright information regarding the original code.
