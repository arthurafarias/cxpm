#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Threading/ThreadPool.hpp"
#include <Containers/Tuple.hpp>

#include <format>
#include <future>
#include <memory>
#include <stdexcept>

namespace Utils::Unix {

  using namespace Core::Containers;

class ShellManager {

  explicit ShellManager() = delete;

  ShellManager(const ShellManager &) = delete;
  ShellManager operator=(const ShellManager &) = delete;

  ShellManager(const ShellManager &&) = delete;
  ShellManager operator=(const ShellManager &&) = delete;

public:
  static inline const std::tuple<int, String, String> exec(String command,
                                                           bool shell = false) {

    String result = "";

    char buffer[1024];

    if (shell) {
      command = std::format("/usr/bin/bash -c \"{}\"", command);
    }

    auto fp = ::popen(command.c_str(), "r");

    if (fp == nullptr) {
      throw std::runtime_error(
          std::format("failed to run {}", command).c_str());
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
      result.append_range(buffer);
    }

    auto value = pclose(fp);

    return {value, result, ""};
  }

  static inline const std::shared_ptr<
      std::promise<std::tuple<int, String, String>>>
  exec_async(String command, bool shell = false) {
    using promise_type = std::promise<std::tuple<int, String, String>>;
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
} // namespace Utils::Posix