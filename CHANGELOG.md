# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).
This file is inspired by [Keep a `CHANGELOG`](http://keepachangelog.com/).

## [Unreleased]
### Added
- `Make exports` which outputs shell `export` commands (to avoid global installation)

### Changed
- More conventional directory layout (#4)

## [6.0.0] - 2016-01-04
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

[Unreleased]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v6.0.1...HEAD
[6.0.1]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v6.0.0...v6.0.1
[6.0.0]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v5.1.3...v6.0.0
[5.1.3]: https://github.com/eddieantonio/isri-ocr-evaluation-tools/compare/v5.1.0...v5.1.3
