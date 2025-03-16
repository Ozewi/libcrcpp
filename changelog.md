# libcrc++ library changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

## [1.0] - 2025.03.16

### Changed
- Translated into english.
- Namespace changed to 'libcrc'.
- Lookup table is now an array instead of a pointer.
- Function names changed to camelCase.
- getLookupTable now returns a const pointer.

### Added
- New test program used as example of use.

## [0.2.1] - 2018.11.23

### Changed
- Compute: Pointer to buffer parameter is now a const pointer.

## [0.2.0] - 2017.05.09

### Added
- Documentation.
- Separated templates with and without lookup table.

### Fixed
- Various bugs.

## [0.1.0] - 2017.05.05

### Added
- Testing program.
- Tests and fixes.

### Removed
- Removed all CPP files; the library has become a single-header-only.

## [0.0.2] - 2017.05.04

### Changed
- Shift direction is now a parameter of the template instead of the class constructor.

## [0.0.1] - 2017.05.03

### Added
- First skeleton.
