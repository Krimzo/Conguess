#pragma once

#include "helper.h"


struct tPolygon
{
    std::vector<kl::Float2> coords;

    [[nodiscard]] bool contains( kl::Float2 point ) const;
};

struct Country
{
    std::string name;
    std::vector<tPolygon> polygons;
};

struct ConguessCountryData
{
    Conguess& conguess;

    std::vector<Country> countries;

    ConguessCountryData( Conguess& conguess );

    std::string const& get_name( int index ) const;
};
