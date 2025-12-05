#pragma once

#include <CXPM/Core/Threading/Mutex.hpp>

template<typename MutexType = Mutex>
using UniqueLock = std::unique_lock<MutexType>;