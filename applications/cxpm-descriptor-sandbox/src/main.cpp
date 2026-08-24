// cxpm-descriptor-sandbox: a small, single-purpose helper process. Its only job is to dlopen a
// shared object compiled from a package.cpp/toolchain.cpp, call its generated get_project()/
// get_toolchain() getter, and print the resulting descriptor as compact JSON on stdout.
//
// It exists so that CXPM::Controllers::DescriptorSandbox (see
// CXPM/Controllers/DescriptorSandbox.hpp) never has to dlopen a package.cpp/toolchain.cpp's
// compiled output directly inside the main cxpm process. That code was compiled moments earlier
// from a project- or dependency-authored .cpp file; its static initializers run with whatever
// privileges this process has the instant dlopen() returns. Isolating that step in a disposable
// child process bounded by rlimits and a wall-clock alarm keeps a buggy or malicious
// package.cpp/toolchain.cpp from being able to do anything to the actual `cxpm` process's memory,
// file descriptors, or exit code beyond what this file's stdout/exit-code contract allows.
//
// See docs/SRS-sandbox.md.

#include <CXPM/Modules/Serialization/JsonManifest.hpp>
#include <CXPM/ProjectDescriptor.hpp>
#include <CXPM/ToolchainDescriptor.hpp>

#include <csignal>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <string>

#include <sys/resource.h>
#include <unistd.h>

#if defined(__linux__)
#include <sys/prctl.h>
#endif

namespace {

// SIGALRM handler: intentionally does the bare minimum and only calls _exit(), which is
// async-signal-safe, unlike most of the standard library.
extern "C" void on_timeout(int) { _exit(124); }

int read_positive_int_env(const char *name, int fallback) {
  const char *value = std::getenv(name);
  if (value == nullptr) {
    return fallback;
  }
  int parsed = std::atoi(value);
  return parsed > 0 ? parsed : fallback;
}

// Applies every sandboxing constraint before any untrusted code (the dlopen()'d .so's static
// initializers) ever runs. Best-effort: setrlimit()/prctl() failures are not fatal, since a
// tightened-but-imperfect sandbox is still strictly better than none, and this helper's only
// job is to shrink blast radius, not to be a hard security boundary on its own.
void apply_sandbox_constraints() {
#if defined(__linux__)
  prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
#endif

  struct rlimit no_core {
    0, 0
  };
  setrlimit(RLIMIT_CORE, &no_core);

  int cpu_limit_seconds =
      read_positive_int_env("CXPM_SANDBOX_CPU_LIMIT_SECONDS", 10);
  struct rlimit cpu_limit {
    static_cast<rlim_t>(cpu_limit_seconds),
        static_cast<rlim_t>(cpu_limit_seconds) + 1
  };
  setrlimit(RLIMIT_CPU, &cpu_limit);

  int memory_limit_mb = read_positive_int_env("CXPM_SANDBOX_MEMORY_LIMIT_MB", 512);
  rlim_t memory_limit_bytes =
      static_cast<rlim_t>(memory_limit_mb) * 1024 * 1024;
  struct rlimit address_space_limit {
    memory_limit_bytes, memory_limit_bytes
  };
  setrlimit(RLIMIT_AS, &address_space_limit);

  // A wall-clock backstop independent of RLIMIT_CPU, which only counts CPU time and would
  // never fire against code that hangs blocked on I/O rather than spinning.
  std::signal(SIGALRM, on_timeout);
  alarm(static_cast<unsigned int>(cpu_limit_seconds) + 5);
}

} // namespace

int main(int argc, char *argv[]) {
  apply_sandbox_constraints();

  if (argc != 3) {
    std::cerr << "usage: cxpm-descriptor-sandbox <project|toolchain> "
                 "<shared-object-path>\n";
    return 2;
  }

  std::string kind = argv[1];
  std::string shared_object_path = argv[2];

  if (kind != "project" && kind != "toolchain") {
    std::cerr << "unknown descriptor kind '" << kind
               << "' (expected 'project' or 'toolchain')\n";
    return 2;
  }

  void *handle = dlopen(shared_object_path.c_str(), RTLD_NOW);
  if (handle == nullptr) {
    std::cerr << "couldn't dlopen " << shared_object_path << ": " << dlerror()
               << "\n";
    return 1;
  }

  try {
    using namespace CXPM::Modules::Serialization;

    if (kind == "project") {
      using getter_type = const CXPM::ProjectDescriptor *(*)();
      auto getter =
          reinterpret_cast<getter_type>(dlsym(handle, "get_project"));
      if (getter == nullptr) {
        std::cerr << "missing get_project symbol in " << shared_object_path
                   << "\n";
        dlclose(handle);
        return 1;
      }
      std::cout << write_json(to_json(*getter()));
    } else {
      using getter_type = const CXPM::ToolchainDescriptor *(*)();
      auto getter =
          reinterpret_cast<getter_type>(dlsym(handle, "get_toolchain"));
      if (getter == nullptr) {
        std::cerr << "missing get_toolchain symbol in " << shared_object_path
                   << "\n";
        dlclose(handle);
        return 1;
      }
      std::cout << write_json(to_json(*getter()));
    }
  } catch (const std::exception &ex) {
    std::cerr << "error extracting descriptor from " << shared_object_path
               << ": " << ex.what() << "\n";
    dlclose(handle);
    return 1;
  }

  dlclose(handle);
  return 0;
}
