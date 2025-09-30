### Security
- 

### Added
- kernel: `_kernel_raise_error` is now implemented.

### Changed
- stdlib: `atexit` now handles 32 registered functions (previously only handled 8).
- stdio: Character input from console now echos and returns `\n` for keyboard return (previously only returned raw characters).

### Fixed
- kernel: `_kernel_osfile` and `__osfile2`/`__osfile3` now call the correct SWI (previously called `OS_Args`).

### Removed
- 
