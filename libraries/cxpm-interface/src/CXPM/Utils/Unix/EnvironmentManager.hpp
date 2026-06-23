#pragma once

#include <cstring>
#include <deque>
#include <filesystem>

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Utils::Unix {

class EnvironmentManager {
private:
  explicit EnvironmentManager() = delete;

  EnvironmentManager(const EnvironmentManager &) = delete;
  EnvironmentManager &operator=(const EnvironmentManager &) = delete;

  EnvironmentManager(const EnvironmentManager &&) = delete;
  EnvironmentManager &operator=(const EnvironmentManager &&) = delete;

public:
  static inline BasicCollection<String>
  get(const char *__str) {
    BasicCollection<String> result;
    const auto &__env = getenv(__str);

    if (__env == nullptr) {
      return BasicCollection<String>();
    }
    
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

  inline static const String which(const char *filename) {

    auto folders = BasicCollection<String>();
    folders.append_range(Utils::Unix::EnvironmentManager::get("PATH"));

    if (std::filesystem::path(filename).is_absolute()) {

      if (std::filesystem::exists(filename)) {
        return filename;
      }

      return "";
    }

    for (auto folder : folders) {

      using directory_iterator = std::filesystem::recursive_directory_iterator;

      for (auto file : directory_iterator(folder.c_str())) {

        if (file.path().filename() == filename) {
          return file.path().string();
        }
      }
    }

    return "";
  }

  inline static const String
  which(const String &str) {
    return which(str.c_str());
  }
};
} // namespace Utils::Unix