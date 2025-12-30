### Security
- kernel: `_kernel_call` is now implemented.
- callx: `_callx`, like `_swix` is now implemented.

### Added
- math: `sincos` and `sincosf` are now implemented.

### Changed
- stdio: `sscanf` now supports `%d` and `%i` formats.

### Fixed
- signal: Update signal to have an alias of `__sysv_signal` to satisfy renamed symbols.
- linking: The `.internal` symbol prefix is now ignored, just like the `.hidden` prefix.
- stdio: `fgets` now reads a full line for file handles (previously would read only a single character).
- stdio: `sscanf` now handles literal charactesr (previously would fail to skip over them).

### Removed
- 
