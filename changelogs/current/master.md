### Security
- 

### Added
- 

### Changed
- docs: Project name changed to `riscos64-clib`.
- linking: Overlong function names are now truncated in signatures, rather than having their name omitted entirely.

### Fixed
- stdio: Fix for fgets failing to include the trailing newline in the read line.
- stdio: `printf`/`sprintf`/`fprintf` now does nothing when supplied with `NULL` as the format string (previously crashed).
- kernel: Ensured that the SWIs are correctly flagged as functions in assembler.

### Removed
- 
