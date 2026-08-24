#[[
CxpmAddCliTest.cmake

The CTest-based tooling facility for exercising the cxpm command-line tool
end to end. See docs/SRS-ctest-tooling.md for the requirements this module
implements and docs/testing.md for how to run the resulting tests.

Every function here is a thin, reusable wrapper around add_test()/
set_tests_properties() so that CLI-level tests read as declarative
statements ("run this cxpm command against this fixture and expect this
exit code") instead of hand-rolled shell scripts. All commands run through
CTest, so `ctest -R <regex>`, `ctest -L <label>`, `ctest -j<n>` and
`ctest --output-on-failure` all work uniformly for both unit and CLI tests.
]]

include_guard(GLOBAL)

# cxpm_add_cli_fixture(<fixture-name> SOURCE_DIRECTORY <dir>)
#
# Registers a SETUP/CLEANUP fixture pair named <fixture-name> that copies
# SOURCE_DIRECTORY into an isolated, per-fixture temporary directory under
# the build tree before any test requiring the fixture runs, and removes it
# afterwards. Sets <fixture-name>_DIRECTORY in the parent scope to the path
# tests should point the cxpm command at.
function(cxpm_add_cli_fixture FIXTURE_NAME)
  cmake_parse_arguments(ARG "" "SOURCE_DIRECTORY" "" ${ARGN})
  if(NOT ARG_SOURCE_DIRECTORY)
    message(FATAL_ERROR "cxpm_add_cli_fixture(${FIXTURE_NAME}): SOURCE_DIRECTORY is required")
  endif()

  set(_workdir "${CMAKE_CURRENT_BINARY_DIR}/cli-fixtures/${FIXTURE_NAME}")

  add_test(NAME "fixture:${FIXTURE_NAME}:setup"
           COMMAND ${CMAKE_COMMAND}
                   -E copy_directory "${ARG_SOURCE_DIRECTORY}" "${_workdir}")
  set_tests_properties("fixture:${FIXTURE_NAME}:setup" PROPERTIES
    FIXTURES_SETUP "${FIXTURE_NAME}"
    LABELS "cli;fixture")

  add_test(NAME "fixture:${FIXTURE_NAME}:cleanup"
           COMMAND ${CMAKE_COMMAND} -E rm -rf "${_workdir}")
  set_tests_properties("fixture:${FIXTURE_NAME}:cleanup" PROPERTIES
    FIXTURES_CLEANUP "${FIXTURE_NAME}"
    LABELS "cli;fixture")

  set("${FIXTURE_NAME}_DIRECTORY" "${_workdir}" PARENT_SCOPE)
endfunction()

# cxpm_add_cli_test(<test-name>
#                    COMMAND <arg...>
#                    [WORKING_DIRECTORY <dir>]
#                    [FIXTURES <fixture-name...>]
#                    [LABELS <label...>]
#                    [TIMEOUT <seconds>]
#                    [WILL_FAIL]
#                    [PASS_REGULAR_EXPRESSION <regex>]
#                    [FAIL_REGULAR_EXPRESSION <regex>])
#
# Registers a CTest test that runs the cxpm executable (or any COMMAND) as a
# child process and asserts on its exit code and, optionally, its output.
# COMMAND may reference $<TARGET_FILE:cxpm> to resolve the freshly-built
# binary regardless of the generator's output layout.
function(cxpm_add_cli_test TEST_NAME)
  cmake_parse_arguments(ARG
    "WILL_FAIL"
    "WORKING_DIRECTORY;TIMEOUT;PASS_REGULAR_EXPRESSION;FAIL_REGULAR_EXPRESSION"
    "COMMAND;FIXTURES;LABELS"
    ${ARGN})

  if(NOT ARG_COMMAND)
    message(FATAL_ERROR "cxpm_add_cli_test(${TEST_NAME}): COMMAND is required")
  endif()
  if(NOT ARG_TIMEOUT)
    set(ARG_TIMEOUT 60)
  endif()

  add_test(NAME "${TEST_NAME}" COMMAND ${ARG_COMMAND})

  set(_labels "cli" ${ARG_LABELS})
  set_tests_properties("${TEST_NAME}" PROPERTIES
    TIMEOUT "${ARG_TIMEOUT}"
    LABELS "${_labels}")

  if(ARG_WORKING_DIRECTORY)
    set_tests_properties("${TEST_NAME}" PROPERTIES
      WORKING_DIRECTORY "${ARG_WORKING_DIRECTORY}")
  endif()
  if(ARG_FIXTURES)
    set_tests_properties("${TEST_NAME}" PROPERTIES
      FIXTURES_REQUIRED "${ARG_FIXTURES}")
  endif()
  if(ARG_WILL_FAIL)
    set_tests_properties("${TEST_NAME}" PROPERTIES WILL_FAIL TRUE)
  endif()
  if(ARG_PASS_REGULAR_EXPRESSION)
    set_tests_properties("${TEST_NAME}" PROPERTIES
      PASS_REGULAR_EXPRESSION "${ARG_PASS_REGULAR_EXPRESSION}")
  endif()
  if(ARG_FAIL_REGULAR_EXPRESSION)
    set_tests_properties("${TEST_NAME}" PROPERTIES
      FAIL_REGULAR_EXPRESSION "${ARG_FAIL_REGULAR_EXPRESSION}")
  endif()
endfunction()
