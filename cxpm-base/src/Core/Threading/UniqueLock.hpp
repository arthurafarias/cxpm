#pragma once

#include <Core/Threading/Mutex.hpp>

template<typename MutexType = Mutex>
using UniqueLock = std::unique_lock<MutexType>;