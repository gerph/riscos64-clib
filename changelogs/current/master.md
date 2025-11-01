### Security
- 

### Added
- maths: `frexp`, `frexpf`, `frexpl` functions.
- string: `strcoll`, `strxfrm` functions.
- time: `difftime`, `mktime` functions.
- stdio: `fscanf`, `vfscanf` functions.

### Changed
- 

### Fixed
- stdlib: `rand()` no longer returns the same value on every call.
- kernel: Corrected name of `_kernel_NONX` in `kernel.h` header.
- swis: `_swi` now returns the correct register when `_RETURN()` is used.
- stdio: `fputs` no longer appends a newline after the string.

### Removed
- 
