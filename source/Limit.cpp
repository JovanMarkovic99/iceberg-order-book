
#include "../include/Limit.hpp"

#include <cassert>

namespace jvn
{
    Limit::Limit(limit_type limit) :
        m_limit(limit) {
        assert(limit >= 0);
    }
}