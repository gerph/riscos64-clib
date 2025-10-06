#!/bin/bash
##
# Test that we can install and run the components we need.
#
# Syntax:
#   ./test-install.sh <targetdir> <export>
#

scriptdir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
rootdir="$(cd "$scriptdir/../../" && pwd -P)"
logdir="$rootdir/logs"

set -e
set -o pipefail

TARGETDIR=${1:-testwork}
EXPORTDIR=${2:-${scriptdir}/../export}
XML=${3:-$logdir/test-install.xml}


function canonicalise() {
    local f=$1
    local dir
    dir="$(cd "$(dirname "$f")" > /dev/null && pwd -P)"
    if [[ "$dir" == '/' ]] ; then
        echo "/$(basename "$f")"
    else
        echo "$dir/$(basename "$f")"
    fi
}
if [[ "${XML:0:1}" != '/' ]] ; then
    XML=$(canonicalise "$XML")
fi
if [[ "${TARGETDIR:0:1}" != '/' ]] ; then
    TARGETDIR="$(canonicalise "$PWD/$TARGETDIR")"
fi

source "${scriptdir}/../../tests/junitxml.sh"
source "${scriptdir}/../../tests/progress.sh"
junitxml_tempdir ""

BIN=${TARGETDIR}/bin

echo "Clearing out directory '$TARGETDIR' before tests"
rm -rf "${TARGETDIR}"/* || true

echo "Creating directory '${BIN}' for tools"
mkdir -p "${BIN}"

cd "${TARGETDIR}"
mkdir -p "${logdir}"

junitxml_testsuite "Installation tests"
junitxml_ci_properties


# Are we done
all_done=false
in_test=false


function cleanup() {
    local rc=$?
    if $in_test ; then
        result "fail"
    fi
    if ! $all_done ; then
        finish
    fi

    return $rc
}

trap cleanup EXIT


##
# Finish all our processing and exit if error.
function finish() {
    all_done=true
    junitxml_report "$XML"
    junitxml_cleanup

    # Report the results
    #"${scriptdir}/../less/junitxml.py" --show --summarise "$XML" || true

    if [[ "$junitxml_nfail" != 0 ]] ; then
        error "$junitxml_nfail tests failed ($junitxml_npass passed)"
    else
        step "$junitxml_nfail tests failed ($junitxml_npass passed)"
    fi
}


##
# Report the start of the test's run
function start() {
    local name="$1"
    step "Test: ${name}"
    junitxml_start "$name"
    in_test=true
}


##
# Report the result of the test's run
function result() {
    local result="$1"
    in_test=false
    if [[ "$result" = 'pass' ]] ; then
        success "Passed"
    else
        notice "FAIL"
    fi
    junitxml_result "$result"
}



start "Install binaries"
# Install the binaries so that we can test things
if "${EXPORTDIR}/bin/robuild64" install "${BIN}" 2>&1 | junitxml_output ; then
    result "pass"
else
    result "fail"
    finish
fi

# Check that we can run the shell with commands
start "Check shell commands"
if [[ "$(bin/riscos64-shell echo '$CT_VERSION')" = '' ]] ; then
    result "fail"
else
    result "pass"
fi

# NOTE: Shell tool cannot take input (ie no pipe like `echo whoami | riscos-shell`)
# Check that it can take input
#start "Input to shell commands"
#if [[ "$(echo 'whoami' | ${BIN}/riscos-shell 2>&1 | junitxml_output)" != "$(whoami)" ]] ; then
#    result "fail"
#else
#    result "pass"
#fi

# (other build environment tooling checks skipped)


# 64bit binaries checking
if [[ -f "${BIN}/riscos64-cc" ]] ; then

    # C code in non-RISC OS form
    mkdir -p absolute
    cd absolute
    cp "${scriptdir}/main.c" main.c
    start "Check riscos64-cc, riscos64-link (non-RISC OS)"
    output=$("${BIN}/riscos64-cc" main.c 2>&1 | junitxml_output)
    if [[ ! -f main,ff8 ]]; then
        result "fail"
    else
        result "pass"
    fi

    cd ..
fi


# We're all done, report results
finish
