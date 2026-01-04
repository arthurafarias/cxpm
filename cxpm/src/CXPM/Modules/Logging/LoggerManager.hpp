#pragma once

#include "CXPM/Modules/Console/Colors/ANSIColorCode.hpp"
#include "CXPM/Modules/Console/Colors/Utils/paint_string.hpp"
#include <atomic>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <syncstream>

namespace CXPM::Modules::Logging {

class LoggerManager {
  LoggerManager() = delete;

  LoggerManager(const LoggerManager &) = delete;
  LoggerManager operator=(const LoggerManager &) = delete;

  LoggerManager(const LoggerManager &&) = delete;
  LoggerManager operator=(const LoggerManager &&) = delete;

public:
  enum class Level { Info = 0, Warn = 1, Debug = 2, Max };

  template <typename... ArgsTypes>
  static void log(const String &prefix,
                  const std::format_string<ArgsTypes...> &fmt,
                  ArgsTypes &&...args) {
    using namespace std::chrono;
    auto now = std::format("{:%Y-%m-%d %H:%M:%S}", system_clock::now());
    std::osyncstream(*stream_current())
        << now << ": " << prefix << ": "
        << std::format(fmt, std::forward<ArgsTypes>(args)...) << std::endl;
  }

  template <typename... ArgsTypes>
  static void info(const std::format_string<ArgsTypes...> &fmt,
                   ArgsTypes &&...args) {
    using namespace CXPM::Modules::Console::Colors::Utils;
    if (_level < Level::Info) {
      return;
    }

    log(paint_string("INFO", Console::Colors::ANSIColorCode::Blue), fmt,
        std::forward<ArgsTypes>(args)...);
  }

  template <typename... ArgsTypes>
  static void warning(const std::format_string<ArgsTypes...> &fmt,
                      ArgsTypes &&...args) {
    using namespace CXPM::Modules::Console::Colors::Utils;
    if (_level < Level::Warn) {
      return;
    }

    log(paint_string("WARNING", Console::Colors::ANSIColorCode::Yellow), fmt,
        std::forward<ArgsTypes>(args)...);
  }

  template <typename... ArgsTypes>
  static void debug(const std::format_string<ArgsTypes...> &fmt,
                    ArgsTypes &&...args) {
    using namespace CXPM::Modules::Console::Colors::Utils;
    if (_level < Level::Debug) {
      return;
    }
    log(paint_string("DEBUG", Console::Colors::ANSIColorCode::Green), fmt,
        std::forward<ArgsTypes>(args)...);
  }

  template <typename... ArgsTypes>
  static void error(const std::format_string<ArgsTypes...> &fmt,
                    ArgsTypes &&...args) {
    using namespace CXPM::Modules::Console::Colors::Utils;
    log(paint_string("ERROR", Console::Colors::ANSIColorCode::Red), fmt,
        std::forward<ArgsTypes>(args)...);
  }

  static constexpr void level_set(const LoggerManager::Level &level) {
    _level = level;
  }

  static constexpr void stream_set(std::shared_ptr<std::ostream> stream) {

    if (stream == nullptr) {
      return;
    }

    _stream_current = stream;
  }

  static constexpr std::shared_ptr<std::ostream> stream_default() {
    return stream_file();
  }

  static constexpr std::shared_ptr<std::ostream> stream_file() {

    if (_stream_default == nullptr) {

      if (std::filesystem::exists("log.txt")) {
        std::filesystem::copy_file(
            "log.txt",
            std::format("log.{}.txt",
                        std::format("{:%Y-%m-%d.%H:%M:%S}",
                                    std::chrono::system_clock::now())));
      }

      _stream_default = std::make_shared<std::fstream>(
          "log.txt", std::ios::app | std::ios::out);
    }

    return _stream_default;
  }

  static constexpr std::shared_ptr<std::ostream> stream_cout() {
    return std::shared_ptr<std::ostream>(&std::cout, [](std::ostream *) {});
  }

  static constexpr std::shared_ptr<std::ostream> stream_cerr() {
    return std::shared_ptr<std::ostream>(&std::cerr, [](std::ostream *) {});
  }

  static constexpr std::shared_ptr<std::ostream> stream_current() {

    if (_stream_current == nullptr) {
      _stream_current = stream_default();
    }

    return _stream_current;
  }

private:
  static inline std::atomic<Level> _level = Level::Debug;
  static inline std::shared_ptr<std::ostream> _stream_default;
  static inline std::shared_ptr<std::ostream> _stream_current;
};
} // namespace CXPM::Modules::Logging