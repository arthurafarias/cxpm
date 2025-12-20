#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Logging/Manager.hpp"
#include <Core/Containers/Tuple.hpp>
#include <Core/Threading/ThreadPool.hpp>

#include <cstring>
#include <future>
#include <memory>
#include <stdexcept>

using namespace Core::Containers;
namespace Utils {
namespace Unix {

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
      command = String::format("/usr/bin/bash -c \"{}\"", command.c_str());
    }

    Core::Logging::Logger::debug("{}", command.c_str());

    if (!dry) {
      auto fp = ::popen(command.c_str(), "r");

      if (fp == nullptr) {
        throw std::runtime_error(
            String::format("failed to run {}", command).c_str());
      }

      while (fgets(buffer, sizeof(buffer), fp)) {
        std::copy(buffer, buffer + strlen(buffer), std::back_inserter(result));
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

    Core::Threading::ThreadPool::get_instance().submit(
        [command, shell, promise]() {
          auto retval = exec(command, shell);
          promise->set_value(retval);
        });

    return promise;
  }

  //   static inline const std::future<Tuple<int, String, String>>
  //   exec_pool(String command, bool shell = false) {}
};
} // namespace Unix
} // namespace Utils