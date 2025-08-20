### Security
- 

### Added
- 

### Changed
- docs: Project name changed to `riscos64-clib`.
- docs: riscos64-mkreloc now includes documentation for the relocation table format.
- linking: Overlong function names are now truncated in signatures, rather than having their name omitted entirely.
- linking: R/W regions within absolutes and modules are now aligned to a page boundary.
- linking: Relocation table will now be created in a compact format so that it takes less space for most binaries.
- linking: Relocation code may be replaced by creating a routine `__RelocCode`.

### Fixed
- stdio: Fix for fgets failing to include the trailing newline in the read line.
- kernel: Ensured that the SWIs are correctly flagged as functions in assembler.
- linking: More fixes for the relocation table, now allowing the relocation of symbols referencing the .rodata section, and references from the .data.rel.ro section.

### Removed
- 
