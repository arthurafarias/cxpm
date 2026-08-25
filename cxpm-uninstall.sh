#!/usr/bin/env bash
#
# cxpm-uninstall.sh - removes everything cxpm-install.sh installs, from the same prefix.
#
# Only ever touches paths namespaced under cxpm (bin/cxpm, bin/cxpm-descriptor-sandbox,
# include/CXPM, share/cxpm) - never a bare rm -rf of shared directories like <prefix>/include.

set -euo pipefail

PREFIX="${PREFIX:-/usr/local}"
DRY_RUN=0

usage() {
  cat <<EOF
Usage: $(basename "$0") [options]

Options:
  --prefix PATH   Prefix cxpm was installed to (default: \$PREFIX or /usr/local)
  --dry-run       Print what would be removed without removing anything
  -h, --help      Show this help

Uses sudo automatically if the prefix isn't writable by the current user.
EOF
}

while [ $# -gt 0 ]; do
  case "$1" in
    --prefix) PREFIX="$2"; shift 2 ;;
    --prefix=*) PREFIX="${1#*=}"; shift ;;
    --dry-run) DRY_RUN=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) echo "error: unknown option '$1'" >&2; usage >&2; exit 1 ;;
  esac
done

TARGETS=(
  "$PREFIX/bin/cxpm"
  "$PREFIX/bin/cxpm-descriptor-sandbox"
  "$PREFIX/include/CXPM"
  "$PREFIX/share/cxpm"
)

SUDO=""
if [ "$(id -u)" -ne 0 ] && [ -e "$PREFIX" ] && [ ! -w "$PREFIX" ]; then
  SUDO="sudo"
  echo "==> $PREFIX not writable by $(whoami); using sudo"
fi

for target in "${TARGETS[@]}"; do
  if [ -e "$target" ] || [ -L "$target" ]; then
    if [ "$DRY_RUN" -eq 1 ]; then
      echo "would remove: $target"
    else
      echo "==> Removing $target"
      $SUDO rm -rf "$target"
    fi
  fi
done

if [ "$DRY_RUN" -eq 0 ]; then
  echo "==> cxpm uninstalled from $PREFIX"
fi
