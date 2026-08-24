#pragma once

#include <atomic>
#include <filesystem>
#include <string>
#include <string_view>

namespace CXPM::Testing {

inline std::filesystem::path temporary_directory(std::string_view name) {
  static std::atomic<unsigned long long> counter{0};
  auto path = std::filesystem::temp_directory_path() /
              ("cxpm-test-" + std::string{name} + "-" +
               std::to_string(counter.fetch_add(1)));
  std::filesystem::remove_all(path);
  std::filesystem::create_directories(path);
  return path;
}

} // namespace CXPM::Testing
