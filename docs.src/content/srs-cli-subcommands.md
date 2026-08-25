---
title: "SRS: Git-Style Subcommand CLI"
---

# Software Requirements Specification
## A Git-Style Subcommand CLI, and a Reusable `ProgramOptions` Command-Line Framework

**Project:** cxpm
**Scope:** `CXPM::Modules::ProgramOptions::*` (`OptionDescriptor`, `OptionsDescriptorCollection`,
`CommandDescriptor`, `CommandRegistry`, `ParsedCommand`, `CommandLineParser`, `HelpFormatter`),
`Views::ApplicationView`
**Companion documents:** [srs-architecture.md](srs-architecture.md) (items A16, A35 — the dead
option-descriptor schema and the hand-duplicated `print_usage()` this SRS resolves),
[srs-generate.md](srs-generate.md) (the `generate` command's own content requirements, unchanged
by this SRS — only its CLI syntax moves), [tooling.md](tooling.md) (CLI reference)

---

## 1. Purpose

Before this change, `cxpm` was a flat-flag CLI: every operation was a top-level `--flag`
(`--build`, `--install`, `--generate`, each with a single-letter alias), all recognized by a single
undifferentiated `ProgramOptions::Parse` call that returned one flat `Map<String,
BasicCollection<String>>` with no notion of "which flags are valid together." `ApplicationView::run()`
then manually checked `options.contains("build") || options.contains("b")` and so on, in a growing
chain of near-identical `if` blocks. A schema type for describing options
(`ProgramOptions::OptionDescriptor`/`OptionsDescriptorCollection`) already existed but was
never used: `ApplicationView::run()` constructed one and immediately discarded it
([srs-architecture.md](srs-architecture.md) items A16, A35), and `print_usage()` was a separately
hand-maintained string that could (and did) drift from what the flag-checking code actually
accepted.

This SRS does two things:

1. Changes the CLI's shape from flat flags to git-style subcommands: `cxpm <command> [<args>]`
   (`cxpm build .`, `cxpm install . --prefix /usr/local`, `cxpm generate package-json .`,
   `cxpm help`), matching the surface every reader already knows from `git`.
2. Replaces the ad hoc flag-checking in `ApplicationView::run()` with an actual, reusable
   command-line framework in `cxpm-interface` (the core library, not the `cxpm` application):
   `CommandDescriptor`/`CommandRegistry` declare *what* a command accepts, `CommandLineParser`
   turns argv into a validated `ParsedCommand` against that declaration, and `HelpFormatter`
   renders both the top-level command list and any one command's detailed usage directly from
   the same declaration — so the schema and the behavior can no longer drift apart, because the
   behavior *is* the schema now.

This is a breaking CLI change: the old flag-style invocation (`cxpm --build .`, `-b`, `-i`, `-g`)
is removed outright, with no backward-compatible shim. cxpm is, in its own documentation's words,
"an early conceptual prototype, audited honestly" (see the project overview) — there is exactly
one caller of this CLI in the repository (the CTest suite, updated alongside this change) and no
external users to break silently.

## 2. Requirements

### 2.1 CLI contract

1. `cxpm <command> [<args>]` SHALL be the invocation shape. The first non-flag token SHALL select
   a command; every token after it SHALL be parsed against *that command's own* declared options,
   not a single global set.
2. The recognized commands SHALL be exactly: `build`, `install`, `generate`, `help`.
   - `cxpm build <directory>` — unchanged behavior from the old `-b|--build`
     ([srs-architecture.md](srs-architecture.md) AR-1..AR-8 still describe the build pipeline
     itself; only the token that invokes it changes).
   - `cxpm install <directory> [--prefix <path>]` — unchanged behavior from the old
     `-i|--install`/`--prefix`. `--prefix` SHALL default to `/usr/local` when omitted; this
     default SHALL be declared once, on the `install` command's `OptionDescriptor` schema
     (`value_default`), not re-hardcoded at the `ApplicationView::run()` call site the way the old
     flag-checking code did.
   - `cxpm generate <kind> [directory] [--force]` — unchanged behavior and content requirements
     from [srs-generate.md](srs-generate.md); only its invocation syntax moves from
     `-g|--generate <kind> [directory]` to a bare `generate` subcommand.
   - `cxpm help [<command>]` — prints the top-level command list, or (given a command name) that
     command's detailed usage.
3. `cxpm` with no arguments SHALL print the top-level usage and exit non-zero (`1`) — a bare
   invocation is not the same as asking for help.
4. `cxpm -h`, `cxpm --help`, and `cxpm help` (with no further argument) SHALL each print the same
   top-level usage and exit `0`.
5. `cxpm help <command>`, `cxpm <command> -h`, and `cxpm <command> --help` SHALL each print that
   command's detailed usage (its full invocation line, its positional-argument placeholder, and
   every declared option with its short form, default, and description) and exit `0`. `-h`/
   `--help` SHALL be accepted on every command without needing to be declared in that command's
   own option schema.
6. An unrecognized first token SHALL raise `RuntimeException` with the message `cxpm: '<token>' is
   not a cxpm command. See 'cxpm help'.` — caught by `main()`'s existing top-level handler
   ([srs-architecture.md](srs-architecture.md) AR-7's contract is unchanged: exceptions, not error
   codes, and a non-zero exit).
7. A token that looks like an option (`-x`/`--xyz`) but is not declared on the *matched* command's
   schema, and is not the reserved `-h`/`--help`, SHALL raise `RuntimeException` naming the option
   (with its `-`/`--` spelling reconstructed) and the command, and pointing at
   `cxpm help <command>`.
8. A command's own missing required positional argument (e.g. `cxpm build` with no directory,
   `cxpm generate` with no `kind`) SHALL raise `RuntimeException` with the same messages
   `ApplicationView` already raised pre-change — this SRS does not change *what* is required per
   command, only how the CLI routes tokens to that check.

### 2.2 `ProgramOptions` framework (core library)

The following new types live under `CXPM::Modules::ProgramOptions`, alongside the pre-existing
`OptionDescriptor`/`OptionsDescriptorCollection`/`Parse` (the low-level flat tokenizer, unchanged
and still directly unit-tested by `ParseTest`; the framework below is built *on top of* it rather
than replacing it):

1. **`OptionDescriptor`** (existing type, trimmed): `name`, `name_short`, `value_default`,
   `description`. The previously-declared `value` field is removed: a descriptor is now purely a
   *schema* (what a command's author declares up front); a parsed invocation's actual values live
   on `ParsedCommand`, not mutated back onto the shared schema object.
2. **`OptionsDescriptorCollection::find(key)`** (added): resolves a raw token key — as produced by
   `ProgramOptions::Parse`, i.e. with any leading `--`/`-` already stripped — against either an
   option's long or short name, returning `nullptr` if neither matches.
3. **`CommandDescriptor`**: one subcommand's schema — `name` (the token typed by a user),
   `description` (one line, used in the top-level command list), `arguments_usage` (a free-form
   placeholder for its positional arguments, e.g. `<directory>`, used only to render usage text),
   and `options` (an `OptionsDescriptorCollection`).
4. **`CommandRegistry`**: an ordered collection of `CommandDescriptor` with `find(name)` by exact
   match. `ApplicationView` builds exactly one registry (`command_registry()`), listing `build`,
   `install`, `generate`, `help`; adding a subcommand means adding one entry there.
5. **`ParsedCommand`**: the result of parsing — `command` (the matched name), `options` (a
   `Map<String, BasicCollection<String>>` already normalized to each option's *long* name, with
   any schema `value_default` filled in for an option the user omitted), and `positionals`.
   Convenience accessors: `has_option`, `option_values`, `option_value` (with a fallback).
6. **`CommandLineParser`**: constructed with a `CommandRegistry`; `parse(tokens)` implements
   §2.1's contract. It expects `tokens` with the program name already stripped (exactly what a
   conventional `argv + 1` would be) — `ApplicationView::run()` is responsible for that, since
   `AbstractApplication::args()` is seeded directly from `argv` (`argv[0]` included).
7. **`HelpFormatter`**: two pure, static string-building functions — `top_level(program_name,
   program_description, registry)` and `command(program_name, descriptor)` — with no I/O, so both
   are unit-testable without capturing stdout. `ApplicationView::print_usage()` and its new
   per-command counterpart wrap these in `std::osyncstream(std::cout) << ...`, the same output
   mechanism the old hand-written string used.

### 2.3 `ApplicationView` dispatch

1. `run()` SHALL: strip `args().front()`; if the remaining tokens are empty, print top-level usage
   and return `1`; otherwise construct the registry, parse, and dispatch on `parsed.command`
   (`help` handled first per §2.1 items 4–5, then `build`/`install`/`generate` each reading their
   directory/kind from `parsed.positionals` and their options from `parsed.options` — replacing
   every `options.contains("build") || options.contains("b")`-shaped check that existed before).
2. The private business-logic methods this dispatch calls (`build`, `install_project`,
   `install_target`, `generate`, `assert_project_directory`) are unchanged by this SRS — only what
   calls them, and how their arguments are extracted from the parsed command line, changes.

## 3. Non-goals

- No backward-compatible flag shim (`--build` silently mapped to `build`, etc.) — see §1's
  rationale.
- No `cxpm version`/`--version` subcommand — not part of the git-like surface requested here, and
  cxpm has no version-numbering scheme of its own yet to report.
- No change to `uninstall`'s status: it remains an undocumented, unwired stub
  ([srs-architecture.md](srs-architecture.md) item A13, deferred) and is deliberately **not**
  added to the new command registry — adding it as a real, listed subcommand belongs to whichever
  change actually implements it, not to a CLI-surface reshuffle.
- `CommandLineParser`/`HelpFormatter` are a small, purpose-built framework for *this* CLI's shape
  (one level of subcommands, no nested sub-subcommands, no positional-arity validation beyond what
  each command's own handler already checks). They are not a general-purpose argument-parsing
  library and do not attempt to be one.
- Every other SRS page that quotes the old `-b|--build`/`-i|--install`/`-g|--generate` syntax in
  its own examples ([srs-generate.md](srs-generate.md), [srs-json-manifests.md](srs-json-manifests.md),
  [srs-sandbox.md](srs-sandbox.md), [srs-parallel-build.md](srs-parallel-build.md),
  [srs-unit-testing.md](srs-unit-testing.md), [srs-ctest-tooling.md](srs-ctest-tooling.md)) is a
  dated record of the spec current at the time it was written, in the same spirit as
  [srs-architecture.md](srs-architecture.md) item A34's "this finding is now moot rather than
  separately patched" — those pages are intentionally left as-is. Only the *living* reference docs
  a user actually runs commands from ([readme.md](https://github.com/arthurafarias/cxpm#readme),
  [tooling.md](tooling.md), [quickstart.md](quickstart.md), the site's front page) are updated to
  the new syntax alongside this change.

## 4. Testing

| Requirement | Verified by |
|---|---|
| §2.2 `CommandLineParser`: unknown command, unknown option, default fill-in, override, short-name resolution, valueless option, empty-argv/`-h`/`--help`/`help` equivalence, `help <command>` positional capture, `<command> -h` option capture | `CommandLineParserTest` (`CXPM/Modules/ProgramOptions/Testing/CommandLineParserTest.hpp`) — pure unit tests against hand-built registries, no subprocess |
| §2.2 `HelpFormatter`: top-level lists every command, a command's usage line embeds its `arguments_usage`, its options (long name, short name, default), and the always-present `-h, --help` line | `HelpFormatterTest` (`CXPM/Modules/ProgramOptions/Testing/HelpFormatterTest.hpp`) |
| §2.1 CLI contract end to end: help variants, unknown command, unknown option, missing-directory errors for `build`/`install`, `generate`'s existing content contract under its new invocation syntax | CTest CLI tests in `tests/cli/CMakeLists.txt`: `cli_help`, `cli_help_short`, `cli_no_arguments_prints_usage_and_fails`, `cli_unknown_command_fails_cleanly`, `cli_unrecognized_option_fails_cleanly`, `cli_help_subcommand_shows_command_usage`, `cli_command_help_flag_shows_command_usage`, plus every pre-existing `cli_build_*`/`cli_install_*`/`cli_generate_*` test updated to the new syntax |
| End-to-end against a real toolchain: `build`/`install` still produce a running, installed executable under the new syntax | `cli_build_example_executable(_json)`/`cli_install_example_executable*` (opt-in integration tier, `CXPM_BUILD_CLI_INTEGRATION_TESTS=ON`) — re-verified manually against a real `g++` toolchain while implementing this change |

## 5. Acceptance criteria

- `cxpm`, `cxpm -h`/`--help`/`help`, `cxpm help <command>`, and `cxpm <command> -h`/`--help` each
  produce the usage text §2.1 specifies, with the exit codes specified there.
- `cxpm build <directory>`, `cxpm install <directory> [--prefix <path>]`, and
  `cxpm generate <kind> [directory] [--force]` behave identically to their pre-change
  `--build`/`--install`/`--generate` counterparts in every way *except* the token that invokes
  them.
- An unknown command and an unknown option each fail cleanly with a message naming what was wrong
  and pointing at `cxpm help`/`cxpm help <command>`, rather than silently falling through to the
  generic top-level usage the old flat-flag dispatch printed for anything it didn't recognize.
- [srs-architecture.md](srs-architecture.md) items A16 and A35 are resolved: `OptionDescriptor`
  and `OptionsDescriptorCollection` are now live schema every command is validated and rendered
  against, and `print_usage()` is generated from that schema rather than a separately maintained
  string.

All of the above hold as of this writing.
