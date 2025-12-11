#pragma once

#include <Core/Containers/String.hpp>
#include <Core/Containers/Map.hpp>

using namespace Core::Containers;

namespace Modules::SQL::Base
{
using QueryResult = Collection<SharedPointer<Map<String, String>>>;
}