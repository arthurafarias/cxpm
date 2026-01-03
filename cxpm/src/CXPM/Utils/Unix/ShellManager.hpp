#pragma once

#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Logging/LoggerManager.hpp>
#include <CXPM/Core/Containers/Tuple.hpp>
#include <CXPM/Core/Threading/ThreadPool.hpp>

#include <cstring>
#include <format>
#include <future>
#include <memory>
#include <stdexcept>

using namespace CXPM::Core::Containers;
namespace CXPM::Utils::Unix {

class ShellManager {

  explicit ShellManager() = delete;

  ShellManager(const ShellManager &) = delete;
  ShellManager operator=(const ShellManager &) = delete;

  ShellManager(const ShellManager &&) = delete;
  ShellManager operator=(const ShellManager &&) = delete;

public:
  static inline const std::tuple<int, Core::Containers::String,
                                 Core::Containers::String>
  exec(Core::Containers::String command, bool dry = false, bool shell = false) {

    Core::Containers::String result = "";

    char buffer[1024];
    ::memset(buffer, '\0', sizeof(buffer));

    if (shell) {
      command = std::format("/usr/bin/bash -c \"{}\"", command);
    }

    Core::Logging::LoggerManager::debug("{}", command);

    if (!dry) {
      auto fp = ::popen(command, "r");

      if (fp == nullptr) {
        throw std::runtime_error(
            std::format("failed to run {}", command));
      }

      while (fgets(buffer, sizeof(buffer), fp)) {
        result.append_range(buffer);
      }

      auto value = pclose(fp);

      // auto trimmed = result.trim();
      auto trimmed = String::trim(result);

      return {value, trimmed, ""};
    }

    return {0, "", ""};
  }

  static inline const std::shared_ptr<std::promise<
      std::tuple<int, Core::Containers::String, Core::Containers::String>>>
  exec_async(Core::Containers::String command, bool shell = false) {
    using promise_type = std::promise<
        std::tuple<int, Core::Containers::String, Core::Containers::String>>;
    auto promise = std::make_shared<promise_type>();

    Threading::ThreadPool::get_instance().submit([command, shell, promise]() {
      auto retval = exec(command, shell);
      promise->set_value(retval);
    });

    return promise;
  }

  //   static inline const std::future<Tuple<int, String, String>>
  //   exec_pool(String command, bool shell = false) {}
};
} // namespace CXPM::Utils::Unix