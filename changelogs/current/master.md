### Security
- 

### Added
- kernel: `_kernel_raise_error` is now implemented.
- math: New functions: `log10`, `log2`, `cbrt`, `cbrtf`, `cbrtl`, `fmax`, `fmaxf`, `fmaxl`, `fmin`, `fminf`, `fminl`, `scalbnf`.
- math: New functions: `cosh`, `coshf`, `sinh`, `sinhf`, `tanh`, `tanhf`.
- math: New functions: `roundl`, `round` `roundf`, `exp`, `expf`, `expm1`, `expm1f`.
- math: New functions: `cimag`, `cimagf`, `cimagl`, `creal`, `crealf`, `creall`.

### Changed
- environment: Zero-init code for absolutes now checks whether there is enough memory before starting.
- stdlib: `atexit` now handles 32 registered functions (previously only handled 8).
- stdio: Character input from console now echos and returns `\n` for keyboard return (previously only returned raw characters).
- signal: Slight memory saving in the escape handler.

### Fixed
- kernel: `_kernel_osfile` and `__osfile2`/`__osfile3` now call the correct SWI (previously called `OS_Args`).
- linking: Hidden symbols are now recognised and not faulted.

### Removed
- 
