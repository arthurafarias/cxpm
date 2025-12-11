#pragma once

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