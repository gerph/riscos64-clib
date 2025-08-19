### Security
- 

### Added
- 

### Changed
- docs: Project name changed to `riscos64-clib`.
- linking: Overlong function names are now truncated in signatures, rather than having their name omitted entirely.

### Fixed
- stdio: Fix for fgets failing to include the trailing newline in the read line.
- kernel: Ensured that the SWIs are correctly flagged as functions in assembler.
- linking: More fixes for the relocation table, now allowing the relocation of symbols referencing the .rodata section, and references from the .data.rel.ro section.

### Removed
- 
