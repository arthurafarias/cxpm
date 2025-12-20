#pragma once
namespace Utils {
namespace StringUtils {
struct FormatString {
  FormatString() : fmt(nullptr) {}
  FormatString(const char *fmt) : fmt(fmt) {}
  const char *get() { return fmt; }

private:
  const char *fmt;
};
} // namespace StringUtils
} // namespace Utils