#!/usr/bin/env bash
#
# cxpm-install.sh - standalone install script for cxpm (no CMake).
#
# Builds (via cxpm-build.sh, unless --no-build is given) and installs:
#   <prefix>/bin/cxpm
#   <prefix>/bin/cxpm-descriptor-sandbox
#   <prefix>/include/CXPM/...                       (cxpm-interface public headers)
#   <prefix>/share/cxpm/toolchains/<name>/toolchain.cpp   (one per built-in toolchain)
#
# Toolchain descriptors are installed as *source*, not compiled: cxpm JIT-compiles
# toolchain.cpp into a shared object per-project the first time it's needed (see
# CXPM::Controllers::ToolchainManager::build_toolchain_plugin), using the headers this
# script installs under <prefix>/include.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

PREFIX="${PREFIX:-/usr/local}"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/build-standalone}"
DO_BUILD=1
DEBUG=0
CXX="${CXX:-c++}"

TOOLCHAINS=(g++ gcc clang clang++ nvcc)

usage() {
  cat <<EOF
Usage: $(basename "$0") [options]

Options:
  --prefix PATH     Install prefix (default: \$PREFIX or /usr/local)
  --build-dir PATH  Where cxpm-build.sh places/looks for built binaries
                     (default: ./build-standalone)
  --no-build        Skip building; install whatever is already in --build-dir
  --debug           Passed through to cxpm-build.sh (debug binaries)
  --cxx PATH        Compiler to use when building (default: \$CXX or c++)
  -h, --help        Show this help

Uses sudo automatically if the prefix isn't writable by the current user.
EOF
}

while [ $# -gt 0 ]; do
  case "$1" in
    --prefix) PREFIX="$2"; shift 2 ;;
    --prefix=*) PREFIX="${1#*=}"; shift ;;
    --build-dir) BUILD_DIR="$2"; shift 2 ;;
    --build-dir=*) BUILD_DIR="${1#*=}"; shift ;;
    --no-build) DO_BUILD=0; shift ;;
    --debug) DEBUG=1; shift ;;
    --cxx) CXX="$2"; shift 2 ;;
    --cxx=*) CXX="${1#*=}"; shift ;;
    -h|--help) usage; exit 0 ;;
    *) echo "error: unknown option '$1'" >&2; usage >&2; exit 1 ;;
  esac
done

if [ "$DO_BUILD" -eq 1 ]; then
  BUILD_ARGS=(--prefix "$PREFIX" --build-dir "$BUILD_DIR" --cxx "$CXX")
  [ "$DEBUG" -eq 1 ] && BUILD_ARGS+=(--debug)
  "$SCRIPT_DIR/cxpm-build.sh" "${BUILD_ARGS[@]}"
fi

if [ ! -x "$BUILD_DIR/cxpm" ] || [ ! -x "$BUILD_DIR/cxpm-descriptor-sandbox" ]; then
  echo "error: binaries not found in $BUILD_DIR (run without --no-build, or run cxpm-build.sh first)" >&2
  exit 1
fi

SUDO=""
if [ "$(id -u)" -ne 0 ] && ! ( mkdir -p "$PREFIX" 2>/dev/null && [ -w "$PREFIX" ] ); then
  SUDO="sudo"
  echo "==> $PREFIX not writable by $(whoami); using sudo"
fi

run() { $SUDO "$@"; }

echo "==> Installing binaries to $PREFIX/bin"
run install -Dm755 "$BUILD_DIR/cxpm" "$PREFIX/bin/cxpm"
run install -Dm755 "$BUILD_DIR/cxpm-descriptor-sandbox" "$PREFIX/bin/cxpm-descriptor-sandbox"

echo "==> Installing headers to $PREFIX/include/CXPM"
while IFS= read -r -d '' header; do
  rel="${header#"$SCRIPT_DIR"/libraries/cxpm-interface/src/}"
  run install -Dm644 "$header" "$PREFIX/include/$rel"
done < <(find "$SCRIPT_DIR/libraries/cxpm-interface/src/CXPM" -name '*.hpp' -not -path '*/Testing/*' -print0)

echo "==> Installing toolchain sources to $PREFIX/share/cxpm/toolchains"
for name in "${TOOLCHAINS[@]}"; do
  src="$SCRIPT_DIR/modules/toolchains/$name/src/toolchain.cpp"
  if [ -f "$src" ]; then
    run install -Dm644 "$src" "$PREFIX/share/cxpm/toolchains/$name/toolchain.cpp"
  else
    echo "warning: skipping missing toolchain source $src" >&2
  fi
done

echo "==> cxpm installed to $PREFIX"
echo "    Run 'cxpm --help' to get started (make sure $PREFIX/bin is on your PATH)."
