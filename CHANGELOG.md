# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).
This file is inspired by [Keep a `CHANGELOG`](http://keepachangelog.com/).

## [7.0.1] - 2018-11-21
### Fixed
- Compile error involving inappropriate use of `ssize_t`

## [7.0.0] - 2018-11-21
### Changed
- Changed name from `isri-ocr-evaluation-tools` to `ocreval` ([#21])

## [6.1.2] - 2017-01-04
### Fixed
- Read in UTF-8 characters in `accsum` ([#14])

## [6.1.1] - 2016-02-22
### Fixed
- No longer spuriously abort if inputs are longer than 65,536 characters ([#10])

## [6.1.0] - 2016-01-01
### Added
- `make exports` which outputs shell `export` commands (to avoid global installation)

### Changed
- More conventional directory layout ([#4])

## [6.0.1] - 2016-01-04
### Fixed
- Bug in implementation of [WB6](http://unicode.org/reports/tr29/#WB6)
- Special case U+0020 SPACE ' ' as a graphic character
- Clang warnings

## [6.0.0] - 2016-01-04
### Added
- Word segmentation using [Unicode word boundaries](http://unicode.org/reports/tr29/#Word_Boundaries).

### Changed
- Start following [SemVer](http://semver.org) properly.
- All input and output is in UTF-8
- Fixes to handle non-BMP code points (code points beyond U+FFFF)

### Removed
- `uni2asc` and `asc2uni` (redundant due to change to UTF-8)

## [5.1.3] - 2015-11-15
### Changed
- More idiomatic `make` build system

### Fixed
- Compiles on modern OS X and Ubuntu

[7.0.1]: https://github.com/eddieantonio/ocreval/compare/v7.0.0...v7.0.1
[7.0.0]: https://github.com/eddieantonio/ocreval/compare/v6.1.2...v7.0.0
[6.1.2]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v6.1.1...v6.1.2
[6.1.1]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v6.1.0...v6.1.1
[6.1.0]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v6.0.1...v6.1.0
[6.0.1]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v6.0.0...v6.0.1
[6.0.0]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v5.1.3...v6.0.0
[5.1.3]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v5.1.0...v5.1.3

[#4]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/issues/4
[#10]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/issues/10
[#14]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/issues/14
[#21]: https://github.com/eddieantonio/ocreval/issues/21
