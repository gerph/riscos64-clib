##
# Makefile for RISC OS 64 (AArch64) C library.
#
#
# Update changelog for current branch:
#	- `make change`
#
# View the combined changelog for the current version:
#	- `make changelog`
#
# View the combined changelog for all versions:
#	- `make full-changelog`
#
# Releasing:
#	- `make release-update`
#	- Update the documentation to reflect the new version.
#		- Check the new changelog in changelogs/releases/x.yy.md.
#		- Use `make release-changes` to show changes since the last release.
#	- Commit any remaining changes
#	- `make release-tag`
#

.PHONY: clib absolutes utilities modules

clib:
	cd clib ; make export
absolutes:
	cd absolutes ; make
modules:
	cd modules ; make
utilities:
	cd utilities ; make

# Record a new changelog entry
change:
	@if [ ! -t 1 ] ; then echo "Not in a tty - cannot edit change" >&2 ; false ; fi
	changelogs/cli.py edit

changelog:
	@# Show the current changelog
	@changelogs/cli.py current

full-changelog:
	@# Show the full changelog
	@changelogs/cli.py full-changelog

# Update the version numbers for release
release-update:
	vmanage inc
	@make release-update-really VERSION=$$(grep Module_MajorVersion_CMHG VersionNum | sed -E 's/^.* ([0-9])/\1/')
release-update-really:
	# Collect all the changelog entries into the new release
	changelogs/cli.py collate "${VERSION}"

release-tag:
	@make release-tag-really VERSION=$$(grep Module_MajorVersion_CMHG VersionNum | sed -E 's/^.* ([0-9])/\1/')
release-tag-really:
	@git tag -a "v${VERSION}" -m "Version ${VERSION}" || ( echo "ERROR: git tag failed, so you may need to remove the old tag first" >&2 ; false )


# List the changes since the last release
release-changes:
	git log --color=always --topo-order $$(git tag | sort -rV | head -1)..HEAD
