---
title: Quickstart
---

# Quickstart

This walks through the whole `cxpm` lifecycle — scaffold, build, run, install — on a fresh
project, recorded end to end as an [asciinema](https://asciinema.org) cast. It assumes `cxpm` is
already built (see the [readme](https://github.com/arthurafarias/cxpm#building-and-running) for
build instructions) and reachable on `PATH`.

<div id="quickstart-cast"></div>
<link rel="stylesheet" type="text/css" href="https://cdn.jsdelivr.net/npm/asciinema-player@3.6.3/dist/bundle/asciinema-player.css" />
<script src="https://cdn.jsdelivr.net/npm/asciinema-player@3.6.3/dist/bundle/asciinema-player.min.js"></script>
<script>
  AsciinemaPlayer.create('../casts/quickstart.cast', document.getElementById('quickstart-cast'), {
    theme: 'monokai',
    fit: 'width',
    idleTimeLimit: 2,
  });
</script>

## Following along without the player

```bash
cxpm generate package-json hello-cxpm
cd hello-cxpm
cat package.json
```

`generate package-json` (see [srs-generate.md](srs-generate.md)) writes a starter
`package.json` whose target `name` comes from the directory argument (`hello-cxpm` here) and
whose `sources` already points at `src/main.cpp` — that file itself isn't generated, so create it:

```cpp
// src/main.cpp
#include <cstdio>

int main() {
  printf("Hello from cxpm!\n");
  return 0;
}
```

```bash
cxpm build .
./hello-cxpm
```

`build` (see [tooling.md](tooling.md#building-a-project)) resolves a toolchain for the target's
`language`, JIT-compiles it, then invokes its compiler and linker.

```bash
cxpm install . --prefix ./install
find install -type f | sort
./install/bin/hello-cxpm
```

`install` (see [tooling.md](tooling.md#installing-a-project)) builds first, then copies the
result into a POSIX-aligned prefix: the executable under `bin/`, and a generated
`hello-cxpm.pc` under `lib/pkgconfig/`.

## Notes on the recording

- The cast runs against a real, freshly built `cxpm` — nothing in it is staged or edited output.
- `cxpm_BUILD_EXTRA_MODULES_PATH` (see
  [tooling.md](tooling.md#describing-a-toolchain-toolchaincpp-or-toolchainjson)) points `cxpm` at
  a toolchain search directory outside the normal install-prefix locations, which is how the
  recording finds a `g++` toolchain without requiring a system-wide `cxpm` install first. In a
  normal install (`cmake --install`, which also installs `modules/toolchains/{gcc,g++,nvcc}`),
  this variable isn't needed.
- `cxpm build`/`install` JIT-compile **every** toolchain plugin visible on the search path
  before selecting one (see [srs-architecture.md](srs-architecture.md) for this and other
  itemized flaws) — the recording's search directory contains only `g++` so that startup cost
  doesn't dominate the cast.

## Re-recording the cast

The exact script used to produce [`casts/quickstart.cast`](../casts/quickstart.cast) — including
prompt pacing and the scoped toolchain directory above — lives at
[`misc/record-quickstart-cast.sh`](https://github.com/arthurafarias/cxpm/blob/master/misc/record-quickstart-cast.sh)
in the repository:

```bash
misc/record-quickstart-cast.sh . hugosite/static/casts/quickstart.cast
```
