#pragma once

#include "helper.h"


struct tPolygon
{
    std::vector<kl::Float2> coords;

    bool contains( kl::Float2 point ) const;

    double area() const;
    double spherical_area( double radius ) const;
};

struct Country
{
    std::string name;
    std::vector<tPolygon> polygons;
    double total_area = 0.0;

    void compute_area();
};

struct ConguessCountryData
{
    Conguess& conguess;

    std::vector<Country> countries;

    ConguessCountryData( Conguess& conguess );

    std::string const& get_name( int index ) const;
};
