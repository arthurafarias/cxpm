#!/usr/bin/env bash
# Records the hugosite/static/casts/quickstart.cast demo used by hugosite/content/quickstart.md.
#
# Usage: record-quickstart-cast.sh <repo-root> <output-cast-path>
#
# <repo-root> must already have a built applications/cxpm/cxpm binary
# (e.g. cmake -S . -B build && cmake --build build).
set -euo pipefail

REPO_ROOT="$1"
OUT_CAST="$2"

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

# A toolchain search directory containing only g++, so the demo doesn't spend
# time JIT-compiling the gcc/nvcc plugins it will never select.
TOOLCHAINS_DIR="$WORKDIR/toolchains"
mkdir -p "$TOOLCHAINS_DIR"
cp -r "$REPO_ROOT/modules/toolchains/g++" "$TOOLCHAINS_DIR/g++"

SESSION_SCRIPT="$WORKDIR/session.sh"

cat > "$SESSION_SCRIPT" <<SESSION
#!/usr/bin/env bash
set -e

prompt() {
  printf '\033[1;32m\$\033[0m \033[1m%s\033[0m\n' "\$1"
}

run() {
  prompt "\$1"
  sleep 0.6
  eval "\$1"
  echo
  sleep 1.1
}

comment() {
  printf '\033[2m# %s\033[0m\n\n' "\$1"
  sleep 1.2
}

export PATH="$REPO_ROOT/build/applications/cxpm:\$PATH"
export cxpm_BUILD_EXTRA_MODULES_PATH="$TOOLCHAINS_DIR"

cd "$WORKDIR"

comment "cxpm's own --help"
run 'cxpm --help'

comment "scaffold a new project — package.json, no build step yet"
run 'cxpm --generate package-json hello-cxpm'
run 'cd hello-cxpm'
run 'cat package.json'

mkdir -p src
cat > src/main.cpp <<'CPP'
#include <cstdio>

int main() {
  printf("Hello from cxpm!\n");
  return 0;
}
CPP

comment "the target's sources file, written ahead of time"
run 'cat src/main.cpp'

comment "cxpm --build resolves a toolchain, then compiles and links"
run 'cxpm --build .'

run './hello-cxpm'

comment "cxpm --install builds (if needed) and lays out a POSIX prefix"
run 'cxpm --install . --prefix ./install'
run 'find install -type f | sort'
run './install/bin/hello-cxpm'

sleep 1
SESSION
chmod +x "$SESSION_SCRIPT"

rm -f "$OUT_CAST"
asciinema rec "$OUT_CAST" \
  --command "bash $SESSION_SCRIPT" \
  --title "cxpm quickstart" \
  --idle-time-limit 2 \
  --window-size 100x28 \
  --overwrite

echo "Wrote $OUT_CAST"
