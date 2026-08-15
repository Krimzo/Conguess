#pragma once

#include "klibrary.h"


struct Conguess;

template<typename... Args>
constexpr void log( Args&&... args )
{
    kl::print( args... );
}
