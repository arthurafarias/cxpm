#pragma once

#include "Core/SharedPointer.hpp"
#include "Utils/Unused.hpp"
#include <atomic>
#include <iostream>
#include <mutex>
#include <ostream>

namespace Core {
Logging {

  class Logger {
    Logger() = delete;

    Logger(const Logger &) = delete;
    Logger operator=(const Logger &) = delete;

    Logger(const Logger &&) = delete;
    Logger operator=(const Logger &&) = delete;

  public:
    enum class Level { Info = 0, Warn = 1, Debug = 2, Max };

    template <typename... ArgsTypes>
    static inline void log(const char *prefix, const char *fmt,
                           ArgsTypes &&...args) {
      std::unique_lock<std::mutex> lk(_S_mutex);
      Utils::Unused{prefix, fmt, args...};
      // using namespace std::chrono;
      // auto now = String::format("{:%Y-%m-%d %H:%M:%S}", system_clock::now());
      // std::osyncstream(*stream_current())
      //     << now << ": " << prefix << ": "
      //     << String::format(fmt, std::forward<ArgsTypes>(args)...) <<
      //     std::endl;
    }

    template <typename... ArgsTypes>
    static inline void info(const char *fmt, ArgsTypes &&...args) {

      if (_level < Level::Info) {
        return;
      }

      log("INFO", fmt, std::forward<ArgsTypes>(args)...);
    }

    template <typename... ArgsTypes>
    static inline void warning(const char *fmt, ArgsTypes &&...args) {

      if (_level < Level::Warn) {
        return;
      }

      log("WARNING", fmt, std::forward<ArgsTypes>(args)...);
    }

    template <typename... ArgsTypes>
    static inline void debug(const char *fmt, ArgsTypes &&...args) {

      if (_level < Level::Debug) {
        return;
      }
      log("DEBUG", fmt, std::forward<ArgsTypes>(args)...);
    }

    template <typename... ArgsTypes>
    static inline void error(const char *fmt, ArgsTypes &&...args) {
      log("ERROR", fmt, std::forward<ArgsTypes>(args)...);
    }

    static inline void level_set(const Logger::Level &level) { _level = level; }

    static inline void stream_set(SharedPointer<std::ostream> stream) {

      if (stream == nullptr) {
        return;
      }

      _stream_current = stream;
    }

    static inline SharedPointer<std::ostream> stream_default() {
      return stream_cout();
    }

    static inline SharedPointer<std::ostream> stream_cout() {
      return SharedPointer<std::ostream>(&std::cout, [](std::ostream *) {});
    }

    static inline SharedPointer<std::ostream> stream_cerr() {
      return SharedPointer<std::ostream>(&std::cerr, [](std::ostream *) {});
    }

    static inline SharedPointer<std::ostream> stream_current() {

      if (_stream_current == nullptr) {
        _stream_current = stream_default();
      }

      return _stream_current;
    }

  private:
    static inline std::mutex _S_mutex;
    static inline std::atomic<Level> _level = Level::Debug;
    static inline SharedPointer<std::ostream> _stream_default;
    static inline SharedPointer<std::ostream> _stream_current;
  };
} // namespace Core::Logging
} // namespace Core