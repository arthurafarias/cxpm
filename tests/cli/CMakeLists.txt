# CLI-level CTest tests built with the cxpm_add_cli_test()/cxpm_add_cli_fixture() tooling
# facility from cmake/CxpmAddCliTest.cmake (see docs/SRS-ctest-tooling.md). These run the
# real cxpm executable as a child process and assert on its exit code/output, exactly the
# way a user would invoke it from a shell.
set(CXPM_EXECUTABLE "$<TARGET_FILE:cxpm>")

# --- Fast, hermetic argument-parsing/error-path smoke tests -----------------------------
# None of these reach toolchain discovery/JIT compilation, so they need no compiler on
# PATH beyond the one that already built cxpm itself, and they run in well under a second.

cxpm_add_cli_test(cli_help
    COMMAND ${CXPM_EXECUTABLE} --help
    LABELS "smoke"
    PASS_REGULAR_EXPRESSION "Usage:")

cxpm_add_cli_test(cli_help_short
    COMMAND ${CXPM_EXECUTABLE} -h
    LABELS "smoke"
    PASS_REGULAR_EXPRESSION "Usage:")

cxpm_add_cli_test(cli_no_arguments_prints_usage_and_fails
    COMMAND ${CXPM_EXECUTABLE}
    LABELS "smoke"
    WILL_FAIL TRUE)

cxpm_add_cli_test(cli_build_without_directory_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} build
    LABELS "smoke;regression"
    WILL_FAIL TRUE)

cxpm_add_cli_test(cli_build_nonexistent_directory_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} build /nonexistent-cxpm-test-directory
    LABELS "smoke"
    WILL_FAIL TRUE)

cxpm_add_cli_test(cli_install_without_directory_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} install
    LABELS "smoke;regression"
    WILL_FAIL TRUE)

cxpm_add_cli_test(cli_directory_without_package_cpp_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} build "${CMAKE_CURRENT_BINARY_DIR}"
    LABELS "smoke"
    WILL_FAIL TRUE)

# --- Git-like subcommand dispatch (see docs/srs-cli-subcommands.md) ---------------------

cxpm_add_cli_test(cli_unknown_command_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} frobnicate
    LABELS "smoke;regression"
    WILL_FAIL TRUE)

cxpm_add_cli_test(cli_unrecognized_option_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} build --bogus
    LABELS "smoke;regression"
    WILL_FAIL TRUE)

cxpm_add_cli_test(cli_help_subcommand_shows_command_usage
    COMMAND ${CXPM_EXECUTABLE} help build
    LABELS "smoke"
    PASS_REGULAR_EXPRESSION "cxpm build - ")

cxpm_add_cli_test(cli_command_help_flag_shows_command_usage
    COMMAND ${CXPM_EXECUTABLE} build --help
    LABELS "smoke"
    PASS_REGULAR_EXPRESSION "cxpm build - ")

# --- `cxpm generate` (see docs/SRS-generate.md, docs/srs-cli-subcommands.md) -------------
# `cxpm generate` creates its target directory itself, so these need no fixture/mkdir step --
# except to remove any leftover output from a *previous* ctest run in the same, uncleaned build
# directory, since cli_generate_package_json_writes_file's PASS_REGULAR_EXPRESSION assumes the
# target file does not already exist. FIXTURES_SETUP/FIXTURES_REQUIRED (rather than a plain
# DEPENDS) guarantees this ordering even under `ctest -R`/`ctest -j`.

set(CXPM_GENERATE_TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/generate/pkg-json")

add_test(NAME "fixture:generate:clean"
         COMMAND ${CMAKE_COMMAND} -E rm -rf "${CXPM_GENERATE_TEST_DIR}")
set_tests_properties("fixture:generate:clean" PROPERTIES
    FIXTURES_SETUP "generate_clean"
    LABELS "smoke;fixture")

cxpm_add_cli_test(cli_generate_package_json_writes_file
    COMMAND ${CXPM_EXECUTABLE} generate package-json "${CXPM_GENERATE_TEST_DIR}"
    FIXTURES "generate_clean"
    LABELS "smoke"
    PASS_REGULAR_EXPRESSION "wrote .*package\\.json")

cxpm_add_cli_test(cli_generate_refuses_to_overwrite_without_force
    COMMAND ${CXPM_EXECUTABLE} generate package-json "${CXPM_GENERATE_TEST_DIR}"
    LABELS "smoke;regression"
    WILL_FAIL TRUE)
set_tests_properties(cli_generate_refuses_to_overwrite_without_force PROPERTIES
    DEPENDS cli_generate_package_json_writes_file)

cxpm_add_cli_test(cli_generate_overwrites_with_force
    COMMAND ${CXPM_EXECUTABLE} generate package-json "${CXPM_GENERATE_TEST_DIR}" --force
    LABELS "smoke")
set_tests_properties(cli_generate_overwrites_with_force PROPERTIES
    DEPENDS cli_generate_package_json_writes_file)

cxpm_add_cli_test(cli_generate_unknown_kind_fails_cleanly
    COMMAND ${CXPM_EXECUTABLE} generate not-a-real-kind "${CMAKE_CURRENT_BINARY_DIR}/generate/bogus"
    LABELS "smoke"
    WILL_FAIL TRUE)

# --- Full build fixtures (integration, opt-in) -------------------------------------------
# Exercises real toolchain discovery/JIT compilation/dlopen against the example projects,
# so it needs a working C++ compiler on PATH and is slower; excluded from the default
# "unit"/"smoke" label filters CI runs against (see docs/testing.md). Point cxpm at the
# in-tree toolchain plugins instead of requiring a prior `cmake --install` of cxpm itself.
if(CXPM_BUILD_CLI_INTEGRATION_TESTS)
    cxpm_add_cli_fixture(example_executable SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/executable")

    cxpm_add_cli_test(cli_build_example_executable
        COMMAND ${CXPM_EXECUTABLE} build "${example_executable_DIRECTORY}"
        FIXTURES "example_executable"
        LABELS "integration"
        TIMEOUT 180)
    set_tests_properties(cli_build_example_executable PROPERTIES
        ENVIRONMENT "cxpm_BUILD_EXTRA_MODULES_PATH=${CMAKE_SOURCE_DIR}/modules/toolchains")

    # Same build, described entirely via package.json instead of package.cpp (see
    # docs/SRS-json-manifests.md) -- exercises ProjectManager::load_project()'s JSON branch
    # end to end against a real toolchain, with no manifest compilation/dlopen involved.
    cxpm_add_cli_fixture(example_executable_json SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/executable-json")

    cxpm_add_cli_test(cli_build_example_executable_json
        COMMAND ${CXPM_EXECUTABLE} build "${example_executable_json_DIRECTORY}"
        FIXTURES "example_executable_json"
        LABELS "integration"
        TIMEOUT 180)
    set_tests_properties(cli_build_example_executable_json PROPERTIES
        ENVIRONMENT "cxpm_BUILD_EXTRA_MODULES_PATH=${CMAKE_SOURCE_DIR}/modules/toolchains")

    # `install` builds first, then installs -- exercises ProjectManager::install_target(),
    # including the fix for a real bug found while writing ProjectManagerTest's hermetic
    # fixture-based unit tests (install used to nest its binary/.pc install logic inside the
    # per-include-directory loop, silently installing nothing for a target with none -- not the
    # case here, but this end-to-end test is what actually exercises the *real* build-then-copy
    # pipeline that install_target's unit tests stub out).
    cxpm_add_cli_fixture(example_executable_install SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/executable")
    set(CXPM_INSTALL_PREFIX_DIR "${CMAKE_CURRENT_BINARY_DIR}/install-prefix")

    cxpm_add_cli_test(cli_install_example_executable
        COMMAND ${CXPM_EXECUTABLE} install "${example_executable_install_DIRECTORY}" --prefix "${CXPM_INSTALL_PREFIX_DIR}"
        FIXTURES "example_executable_install"
        LABELS "integration"
        TIMEOUT 180)
    set_tests_properties(cli_install_example_executable PROPERTIES
        ENVIRONMENT "cxpm_BUILD_EXTRA_MODULES_PATH=${CMAKE_SOURCE_DIR}/modules/toolchains")

    cxpm_add_cli_test(cli_install_example_executable_produces_binary_and_pc_file
        COMMAND ${CMAKE_COMMAND}
                -DCXPM_INSTALL_PREFIX=${CXPM_INSTALL_PREFIX_DIR}
                -DCXPM_TARGET_NAME=example-executable
                -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_install.cmake"
        LABELS "integration")
    set_tests_properties(cli_install_example_executable_produces_binary_and_pc_file PROPERTIES
        DEPENDS cli_install_example_executable)

    # --- cxpm-descriptor-sandbox (see docs/SRS-sandbox.md) -----------------------------------
    # Compiles a real toolchain plugin .so fixture (the exact same inputs
    # ToolchainManager::build_toolchain_plugin() would compile) so the sandbox helper's own
    # contract -- not just "a full build happens to still work" -- gets exercised directly.
    set(CXPM_SANDBOX_FIXTURE_SO "${CMAKE_CURRENT_BINARY_DIR}/sandbox-fixture/libtoolchain-g++.so")
    add_custom_command(
        OUTPUT "${CXPM_SANDBOX_FIXTURE_SO}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/sandbox-fixture"
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -fPIC -shared
                -I "${CMAKE_SOURCE_DIR}/libraries/cxpm-interface/src"
                "${CMAKE_SOURCE_DIR}/modules/toolchains/g++/src/toolchain.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/toolchain_loader.cpp"
                -o "${CXPM_SANDBOX_FIXTURE_SO}"
        DEPENDS "${CMAKE_SOURCE_DIR}/modules/toolchains/g++/src/toolchain.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/toolchain_loader.cpp"
        COMMENT "Compiling a toolchain plugin .so fixture for cxpm-descriptor-sandbox tests")
    add_custom_target(cxpm_sandbox_fixture ALL DEPENDS "${CXPM_SANDBOX_FIXTURE_SO}")

    cxpm_add_cli_test(sandbox_extracts_toolchain_descriptor_as_json
        COMMAND $<TARGET_FILE:cxpm-descriptor-sandbox> toolchain "${CXPM_SANDBOX_FIXTURE_SO}"
        LABELS "integration"
        PASS_REGULAR_EXPRESSION "\"name\":\"g\\+\\+\"")

    cxpm_add_cli_test(sandbox_rejects_missing_shared_object
        COMMAND $<TARGET_FILE:cxpm-descriptor-sandbox> toolchain /nonexistent-cxpm-sandbox-fixture.so
        LABELS "integration;smoke"
        WILL_FAIL TRUE)

    cxpm_add_cli_test(sandbox_rejects_mismatched_kind
        COMMAND $<TARGET_FILE:cxpm-descriptor-sandbox> project "${CXPM_SANDBOX_FIXTURE_SO}"
        LABELS "integration"
        WILL_FAIL TRUE)

    cxpm_add_cli_test(sandbox_rejects_unknown_kind_argument
        COMMAND $<TARGET_FILE:cxpm-descriptor-sandbox> not-a-real-kind "${CXPM_SANDBOX_FIXTURE_SO}"
        LABELS "integration;smoke"
        WILL_FAIL TRUE)
endif()
