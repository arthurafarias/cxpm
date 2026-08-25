#!/usr/bin/env bash
#
# cxpm-build.sh - standalone build script for cxpm (no CMake).
#
# Compiles the two binaries cxpm needs:
#   - cxpm                     (the CLI itself)
#   - cxpm-descriptor-sandbox  (helper used to sandbox-load compiled package.cpp/toolchain.cpp
#                                shared objects, see libraries/cxpm-interface/src/CXPM/DescriptorSandbox.hpp)
#
# Toolchain descriptors (modules/toolchains/*/src/toolchain.cpp) are NOT precompiled here: cxpm
# JIT-compiles them on demand from the installed *source* files (see ToolchainManager::autoscan /
# build_toolchain_plugin), so cxpm-install.sh installs their .cpp sources rather than .so files.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

PREFIX="${PREFIX:-/usr/local}"
CXX="${CXX:-c++}"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/build-standalone}"
DEBUG=0
JOBS=1

usage() {
  cat <<EOF
Usage: $(basename "$0") [options]

Options:
  --prefix PATH     Install prefix baked into the binaries (default: \$PREFIX or /usr/local)
                     Must match whatever --prefix cxpm-install.sh is later run with.
  --build-dir PATH  Where to place the compiled binaries (default: ./build-standalone)
  --debug           Build with -g -O0 instead of -O2 (no optimizations, debug symbols)
  --cxx PATH        Compiler to use (default: \$CXX or c++)
  -h, --help        Show this help

Environment variables PREFIX, CXX, BUILD_DIR are honored as defaults for the flags above.
EOF
}

while [ $# -gt 0 ]; do
  case "$1" in
    --prefix) PREFIX="$2"; shift 2 ;;
    --prefix=*) PREFIX="${1#*=}"; shift ;;
    --build-dir) BUILD_DIR="$2"; shift 2 ;;
    --build-dir=*) BUILD_DIR="${1#*=}"; shift ;;
    --cxx) CXX="$2"; shift 2 ;;
    --cxx=*) CXX="${1#*=}"; shift ;;
    --debug) DEBUG=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) echo "error: unknown option '$1'" >&2; usage >&2; exit 1 ;;
  esac
done

if ! command -v "$CXX" >/dev/null 2>&1; then
  echo "error: compiler '$CXX' not found on PATH" >&2
  exit 1
fi

OPT_FLAGS="-O2"
if [ "$DEBUG" -eq 1 ]; then
  OPT_FLAGS="-g -O0"
fi

mkdir -p "$BUILD_DIR"

echo "==> Building cxpm (prefix baked in: $PREFIX)"
"$CXX" -std=gnu++23 -Wall -Wextra $OPT_FLAGS \
  -Dcxpm_BASE_INSTALL_PREFIX=\""$PREFIX"\" \
  -Dcxpm_BASE_SOURCE_PREFIX=\""$PREFIX"\" \
  -I"$SCRIPT_DIR/applications/cxpm/src" \
  -I"$SCRIPT_DIR/libraries/cxpm-interface/src" \
  -o "$BUILD_DIR/cxpm" \
  "$SCRIPT_DIR/applications/cxpm/src/cxpm.cpp"

echo "==> Building cxpm-descriptor-sandbox"
"$CXX" -std=gnu++23 -Wall -Wextra $OPT_FLAGS \
  -I"$SCRIPT_DIR/libraries/cxpm-interface/src" \
  -o "$BUILD_DIR/cxpm-descriptor-sandbox" \
  "$SCRIPT_DIR/applications/cxpm-descriptor-sandbox/src/main.cpp" \
  -ldl

echo "==> Done. Binaries in $BUILD_DIR:"
ls -la "$BUILD_DIR/cxpm" "$BUILD_DIR/cxpm-descriptor-sandbox"
echo
echo "Next: ./cxpm-install.sh --prefix $PREFIX --build-dir $BUILD_DIR"
