#pragma once

// Runs the cxpm-descriptor-sandbox helper (applications/cxpm-descriptor-sandbox) to extract a
// ProjectDescriptor/ToolchainDescriptor from a compiled package.cpp/toolchain.cpp shared object
// out-of-process, instead of dlopen()-ing it directly inside the running cxpm process (which is
// what ProjectManager::load_from_manifest / ToolchainManager::load_toolchain_plugin did before
// this existed, and still do when the sandbox is disabled). See docs/SRS-sandbox.md.
//
// The two processes communicate over stdout/stderr pipes using the JSON descriptor codec
// (JsonManifest.hpp) that also backs package.json/toolchain.json -- the same serializers that
// let a manifest be authored as JSON are what let a sandboxed child hand a manifest back to its
// parent without either process dereferencing a pointer produced by the other.
//
// posix_spawn(), not fork(), is used deliberately: cxpm links a background ThreadPool
// (Core/Threading/ThreadPool.hpp) used by ShellManager::exec_async, and fork()-ing a
// multithreaded process only duplicates the calling thread -- any lock held by another thread
// at fork time stays locked forever in the child. posix_spawn() avoids that hazard entirely.

#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Modules/Serialization/JsonManifest.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/ToolchainDescriptor.hpp"
#include "CXPM/Utils/Macros/StaticClass.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

#ifndef cxpm_SANDBOX_HELPER_PATH
#define cxpm_SANDBOX_HELPER_PATH ""
#endif

#ifndef cxpm_BASE_INSTALL_PREFIX
#define cxpm_BASE_INSTALL_PREFIX "/usr/local"
#endif

namespace CXPM::Controllers {

class DescriptorSandbox final {

  StaticClass(DescriptorSandbox)

public:
  // Enabled by default; CXPM_SANDBOX_DISABLE=1 is an escape hatch for debugging or for
  // environments where posix_spawn()-ing an extra process is genuinely undesirable.
  static inline bool enabled() {
    const char *disable = std::getenv("CXPM_SANDBOX_DISABLE");
    return disable == nullptr || std::string(disable) != "1";
  }

  static inline ProjectDescriptor load_project(const String &shared_object_path) {
    auto output = run("project", shared_object_path);
    using namespace CXPM::Modules::Serialization;
    return project_descriptor_from_json(parse_json(output));
  }

  static inline ToolchainDescriptor
  load_toolchain(const String &shared_object_path) {
    auto output = run("toolchain", shared_object_path);
    using namespace CXPM::Modules::Serialization;
    return toolchain_descriptor_from_json(parse_json(output));
  }

private:
  static inline std::string helper_path() {
    if (const char *override_path = std::getenv("CXPM_SANDBOX_HELPER")) {
      return override_path;
    }

    std::string build_tree_path = cxpm_SANDBOX_HELPER_PATH;
    if (!build_tree_path.empty() && std::filesystem::exists(build_tree_path)) {
      return build_tree_path;
    }

    auto installed_path = std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) /
                          "bin" / "cxpm-descriptor-sandbox";
    if (std::filesystem::exists(installed_path)) {
      return installed_path.string();
    }

    throw Core::Exceptions::RuntimeException(
        "Couldn't locate the cxpm-descriptor-sandbox helper (checked the "
        "build tree, {}, and $CXPM_SANDBOX_HELPER)",
        installed_path.string());
  }

  static inline std::string read_all(int fd) {
    std::string result;
    char buffer[4096];
    ssize_t bytes_read = 0;
    while ((bytes_read = ::read(fd, buffer, sizeof(buffer))) > 0) {
      result.append(buffer, static_cast<std::size_t>(bytes_read));
    }
    return result;
  }

  static inline String run(const std::string &kind,
                           const String &shared_object_path) {
    auto helper = helper_path();

    int stdout_pipe[2] = {-1, -1};
    int stderr_pipe[2] = {-1, -1};
    if (::pipe(stdout_pipe) != 0 || ::pipe(stderr_pipe) != 0) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't create pipes for the descriptor sandbox: {}",
          std::strerror(errno));
    }

    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_adddup2(&actions, stdout_pipe[1], STDOUT_FILENO);
    posix_spawn_file_actions_adddup2(&actions, stderr_pipe[1], STDERR_FILENO);
    posix_spawn_file_actions_addclose(&actions, stdout_pipe[0]);
    posix_spawn_file_actions_addclose(&actions, stdout_pipe[1]);
    posix_spawn_file_actions_addclose(&actions, stderr_pipe[0]);
    posix_spawn_file_actions_addclose(&actions, stderr_pipe[1]);

    // posix_spawn()/execve() require a mutable argv, hence the explicit std::string copies
    // rather than pointing directly at `helper`/`kind`/`shared_object_path`'s own storage.
    std::vector<std::string> argument_storage = {helper, kind,
                                                 std::string(shared_object_path)};
    std::vector<char *> argv;
    for (auto &argument : argument_storage) {
      argv.push_back(argument.data());
    }
    argv.push_back(nullptr);

    pid_t pid = 0;
    int spawn_result = posix_spawn(&pid, helper.c_str(), &actions, nullptr,
                                   argv.data(), environ);
    posix_spawn_file_actions_destroy(&actions);

    ::close(stdout_pipe[1]);
    ::close(stderr_pipe[1]);

    if (spawn_result != 0) {
      ::close(stdout_pipe[0]);
      ::close(stderr_pipe[0]);
      throw Core::Exceptions::RuntimeException(
          "Couldn't spawn the descriptor sandbox helper {}: {}", helper,
          std::strerror(spawn_result));
    }

    auto stdout_text = read_all(stdout_pipe[0]);
    auto stderr_text = read_all(stderr_pipe[0]);
    ::close(stdout_pipe[0]);
    ::close(stderr_pipe[0]);

    int status = 0;
    ::waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      std::string reason =
          WIFSIGNALED(status)
              ? ("killed by signal " + std::to_string(WTERMSIG(status)))
              : ("exit code " +
                 std::to_string(WIFEXITED(status) ? WEXITSTATUS(status) : -1));
      throw Core::Exceptions::RuntimeException(
          "Sandboxed descriptor extraction for {} failed ({}): {}",
          shared_object_path, reason, stderr_text);
    }

    return String(stdout_text);
  }
};

} // namespace CXPM::Controllers
