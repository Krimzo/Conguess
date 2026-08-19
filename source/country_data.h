#pragma once

#include "helper.h"


struct Country
{
    std::string name;
    std::vector<kl::Polygon> polygons;

    double max_poly_area = 0.0;
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
