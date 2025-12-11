#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/SQL/Base/QueryBase.hpp"
#include "Modules/SQL/Base/QueryResult.hpp"

using namespace Core::Containers;

namespace Modules::SQL::Base
{
    class Driver
    {
        public:
        virtual QueryResult query(SharedPointer<QueryBase> query) = 0;
    };
}