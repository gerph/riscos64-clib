### Security
- 

### Added
- math: `sincos` and `sincosf` are now implemented.

### Changed
- 

### Fixed
- signal: Update signal to have an alias of `__sysv_signal` to satisfy renamed symbols.
- linking: The `.internal` symbol prefix is now ignored, just like the `.hidden` prefix.
- stdio: `fgets` now reads a full line for file handles (previously would read only a single character).

### Removed
- 
