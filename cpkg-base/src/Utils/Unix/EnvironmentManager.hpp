#pragma once

#include <cstring>
#include <deque>
#include <filesystem>
#include <string>

namespace Utils::Unix {

class EnvironmentManager {
private:
  explicit EnvironmentManager() = delete;

  EnvironmentManager(const EnvironmentManager &) = delete;
  EnvironmentManager &operator=(const EnvironmentManager &) = delete;

  EnvironmentManager(const EnvironmentManager &&) = delete;
  EnvironmentManager &operator=(const EnvironmentManager &&) = delete;

public:
  static inline std::deque<std::string> get(const char *__str) {
    std::deque<std::string> result;
    const auto &__env = getenv(__str);
    auto _result = strdup(__env);
    auto _ptr = _result;

    auto token = strtok(_ptr, ";:");

    while (token != NULL) {
      result.push_back(token);
      token = strtok(NULL, ";:");
    }

    free(_result);

    return result;
  }

  inline static const std::string which(const char *filename) {

    auto folders = std::deque<std::string>();
    folders.append_range(Utils::Unix::EnvironmentManager::get("PATH"));

    if (std::filesystem::path(filename).is_absolute()) {

      if (std::filesystem::exists(filename)) {
        return filename;
      }

      return "";
    }

    for (auto folder : folders) {

      using directory_iterator = std::filesystem::recursive_directory_iterator;

      for (auto file : directory_iterator(folder)) {

        if (file.path().filename() == filename) {
          return file.path().string();
        }
      }
    }

    return "";
  }

  inline static const std::string which(const std::string &str) {
    return which(str.c_str());
  }
};
} // namespace Utils::Unix