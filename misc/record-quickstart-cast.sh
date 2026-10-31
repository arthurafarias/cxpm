#!/usr/bin/env bash
# Records docs.src/static/casts/quickstart.cast, the demo embedded in docs.src/content/quickstart.md.
#
# Usage: record-quickstart-cast.sh <repo-root> <output-cast-path>
#
# <repo-root> must already have a bootstrapped cxpm (./cxpm-build.sh).
set -euo pipefail

REPO_ROOT="$1"
OUT_CAST="$2"

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

# A toolchain search directory containing only g++, so the demo doesn't spend
# time compiling the plugins of toolchains it will never select.
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

export PATH="$REPO_ROOT/.cxpm/stage0:\$PATH"
export CXPM_HEADERS_PATH="$REPO_ROOT/libraries/cxpm-interface/src"
export CXPM_SANDBOX_HELPER="$REPO_ROOT/.cxpm/stage0/cxpm-descriptor-sandbox"
export cxpm_BUILD_EXTRA_MODULES_PATH="$TOOLCHAINS_DIR"

cd "$WORKDIR"

comment "a library, with a project that tests it and one that uses it"
run 'cxpm project create greeter --type static-library --tests --example'
run 'cd greeter'
run 'find . -type f | sort'
run 'cat package.cpp'

comment "build: the library is installed into the workspace sysroot for the example"
run 'cxpm build'

comment "test: builds the tests project and runs it"
run 'cxpm test'

comment "pack and install, here under a root of our own"
run 'cxpm pack'
run 'cxpm install --prefix /usr --root ./root'
run 'find root -type f | sort'

comment "and remove it again"
run 'cxpm uninstall --prefix /usr --root ./root'

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
