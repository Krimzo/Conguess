#include "conguess.h"


static constexpr kl::RGB BORDER_COLOR = kl::colors::WHITE;

bool tPolygon::contains( kl::Float2 point ) const
{
    bool inside = false;
    const int n = (int) this->coords.size();
    for ( int i = 0; i < n; i++ )
    {
        kl::Float2 const& p1 = coords[i];
        kl::Float2 const& p2 = coords[size_t( i + 1 ) % n];
        if ( ( p1.y > point.y ) == ( p2.y > point.y ) )
            continue;

        const float x_intersect = ( p2.x - p1.x ) * ( point.y - p1.y ) / ( p2.y - p1.y ) + p1.x;
        if ( x_intersect < point.x )
            continue;

        inside = !inside;
    }
    return inside;
}

double tPolygon::area() const
{
    double area = 0.0;
    const size_t n = coords.size();
    for ( size_t i = 0; i < n; i++ )
    {
        auto const& p1 = coords[i];
        auto const& p2 = coords[( i + 1 ) % n];
        area += 0.5 * ( p1.x * p2.y - p2.x * p1.y );
    }
    return kl::abs( area );
}

double tPolygon::spherical_area( double radius ) const
{
    double sum = 0.0;
    const size_t n = coords.size();
    for ( size_t i = 0; i < n; i++ )
    {
        const double lon_prev = kl::to_radians<double>() * coords[( i + n - 1 ) % n].y;
        const double lon_next = kl::to_radians<double>() * coords[( i + 1 ) % n].y;
        sum += ( lon_next - lon_prev ) * kl::sin_d<double>( coords[i].x );
    }
    return kl::abs( sum ) * radius * radius * 0.5;
}

void Country::compute_area()
{
    static constexpr double EARTH_RADIUS = 6371.0;
    max_poly_area = 0.0;
    total_area = 0.0;
    for ( auto& poly : polygons )
    {
        const double poly_area = poly.spherical_area( EARTH_RADIUS );
        max_poly_area = kl::max( max_poly_area, poly_area );
        total_area += poly_area;
    }
}

kl::Int2 coords_to_point( kl::Int2 image_size, kl::Float2 coords );
kl::Float2 point_to_coords( kl::Int2 image_size, kl::Int2 point );
kl::Float4 min_max_coords( tPolygon const& polygon );
kl::RGB int_to4_value_color( int value );
void draw_indices( std::vector<Country> const& countries, kl::Image& out_image );
void read_country_data( std::string_view const& path, std::vector<Country>& out_countries );
void generate_border_map( std::vector<Country> const& countries, std::string_view const& path );
void generate_indices_map( std::vector<Country> const& countries, std::string_view const& path );

ConguessCountryData::ConguessCountryData( Conguess& conguess )
    : conguess( conguess )
{
    static constexpr int MAX_COUNTRY_COUNT = 255;
    static constexpr std::string_view BORDER_MAP_OUT_PATH = "textures/earth_borders.png";
    static constexpr std::string_view INDEX_MAP_OUT_PATH = "textures/earth_indices.png";

    read_country_data( "data/countries.json", countries );
    const size_t country_count = countries.size();
    if ( country_count > MAX_COUNTRY_COUNT )
    {
        log_error( "Error. Max country count is ", MAX_COUNTRY_COUNT, " but ", country_count, " were provided." );
        return;
    }

    log( "Computing country area" );
    for ( auto& country : countries )
        country.compute_area();

    if ( !std::filesystem::exists( BORDER_MAP_OUT_PATH ) )
        generate_border_map( countries, BORDER_MAP_OUT_PATH );
    else
        log( "Border map already exists ", BORDER_MAP_OUT_PATH );

    if ( !std::filesystem::exists( INDEX_MAP_OUT_PATH ) )
        generate_indices_map( countries, INDEX_MAP_OUT_PATH );
    else
        log( "Index map already exists ", INDEX_MAP_OUT_PATH );
}

std::string const& ConguessCountryData::get_name( int index ) const
{
    return countries[index].name;
}

kl::Int2 coords_to_point( kl::Int2 image_size, kl::Float2 coords )
{
    return {
        int( ( coords.y + 180.0f ) / 360.0f * ( image_size.x - 1 ) ),
        int( image_size.y - 1 - ( coords.x + 90.0f ) / 180.0f * ( image_size.y - 1 ) ),
    };
}

kl::Float2 point_to_coords( kl::Int2 image_size, kl::Int2 point )
{
    return {
        ( image_size.y - 1 - point.y ) / ( image_size.y - 1.0f ) * 180.0f - 90.0f,
        point.x / ( image_size.x - 1.0f ) * 360.0f - 180.0f,
    };
}

kl::Float4 min_max_coords( tPolygon const& polygon )
{
    kl::Float4 res = {
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity() };
    for ( auto const& coord : polygon.coords )
    {
        res.x = std::max( res.x, coord.x );
        res.y = std::min( res.y, coord.y );
        res.z = std::min( res.z, coord.x );
        res.w = std::max( res.w, coord.y );
    }
    return res;
}

kl::RGB int_to4_value_color( int value )
{
    int result[4] = {};
    for ( int i = 0; i < 4; i++ )
    {
        result[3 - i] = value % 4;
        value /= 4;
    }
    return {
        byte( result[0] * 85 ),
        byte( result[1] * 85 ),
        byte( result[2] * 85 ),
        byte( result[3] * 85 ),
    };
}

void draw_indices( std::vector<Country> const& countries, kl::Image& out_image )
{
    std::atomic<int> map_counter = 0;
    const int countries_count = (int) countries.size();
    for ( int i = 0; i < countries_count; i++ )
    {
        const kl::RGB index_color = { byte( i + 1 ), 0, 0 };
        for ( auto const& polygon : countries[i].polygons )
        {
            const kl::Float4 square_bounds = min_max_coords( polygon );
            const kl::Int2 top_left = coords_to_point( out_image.size(), kl::Float2{ square_bounds.x, square_bounds.y } );
            const kl::Int2 bottom_right = coords_to_point( out_image.size(), kl::Float2{ square_bounds.z, square_bounds.w } );
            const int width = bottom_right.x - top_left.x + 1;
            const int height = bottom_right.y - top_left.y + 1;
            kl::async_for( 0, width * height, [&]( int i )
                {
                    const kl::Int2 point = top_left + kl::Int2::from_index( i, width );
                    if ( out_image[point] == index_color )
                        return;
                    if ( polygon.contains( point_to_coords( out_image.size(), point ) ) )
                        out_image[point] = index_color;
                } );
        }
        log( "Drawn index data ", ++map_counter, "/", countries_count, " (", countries[i].name, ")" );
    }
}

void read_country_data( std::string_view const& path, std::vector<Country>& out_countries )
{
    log( "Reading country data from ", path );
    const std::string file_data = kl::read_file_string( path );
    if ( file_data.empty() )
    {
        log_error( "Failed to open country data file ", path );
        return;
    }

    kl::json::Object top_object{ file_data };

    kl::Ref<kl::json::Array> features = top_object["features"].as<kl::json::Array>();
    if ( !features )
    {
        log_error( "Failed to load features form json file ", path );
        return;
    }

    for ( auto& feature : *features )
    {
        auto feature_object = feature.as<kl::json::Object>();
        if ( !feature_object )
            continue;

        auto properties_object = ( *feature_object )["properties"].as<kl::json::Object>();
        if ( !properties_object )
            continue;

        auto admin_literal = ( *properties_object )["ADMIN"].as<kl::json::Literal>();
        if ( !admin_literal )
            continue;

        auto opt_str = admin_literal->get_string();
        if ( !opt_str )
            continue;

        auto geometry_object = ( *feature_object )["geometry"].as<kl::json::Object>();
        if ( !geometry_object )
            continue;

        auto geometry_type_object = ( *geometry_object )["type"].as<kl::json::Literal>();
        if ( !geometry_type_object )
            continue;

        auto geometry_type_opt_str = geometry_type_object->get_string();
        if ( !geometry_type_opt_str )
            continue;

        auto coordinates_array = ( *geometry_object )["coordinates"].as<kl::json::Array>();
        if ( !coordinates_array )
            continue;

        Country& country = out_countries.emplace_back();
        country.name = *opt_str;
        const auto save_polygon = [&]( kl::Ref<kl::json::Container> const& array_cont )
            {
                auto array = array_cont.as<kl::json::Array>();
                if ( !array )
                    return;
                auto& country_polygon = country.polygons.emplace_back();
                for ( auto& coord_cont : *array )
                {
                    auto coord_cont_array = coord_cont.as<kl::json::Array>();
                    if ( !coord_cont_array )
                        continue;
                    country_polygon.coords.emplace_back(
                        ( *coord_cont_array )[1]->get_float().value_or( {} ),
                        ( *coord_cont_array )[0]->get_float().value_or( {} ) );
                }
            };
        for ( auto& polygon : *coordinates_array )
        {
            auto polygon_array = polygon.as<kl::json::Array>();
            if ( !polygon_array )
                continue;
            if ( *geometry_type_opt_str == "MultiPolygon" )
            {
                for ( auto& sub_polygon : *polygon_array )
                    save_polygon( sub_polygon );
            }
            else
                save_polygon( polygon_array );
        }
    }
    log_success( "Read ", out_countries.size(), " from ", path );
}

void generate_border_map( std::vector<Country> const& countries, std::string_view const& path )
{
    kl::Image image;
    image.resize( { 8192, 4096 } );
    draw_indices( countries, image );
    log( "Filtering border data" );
    const kl::Image image_copy = image;
    kl::async_for( 0, image_copy.pixel_count(), [&]( int i )
        {
            const kl::Int2 point = kl::Int2::from_index( i, image_copy.width() );
            const kl::RGB index_color = image_copy[point];
            if ( index_color == kl::RGB{} )
                return;
            for ( kl::Int2 offset{ -1 }; offset.y <= 1; offset.y++ )
            {
                for ( offset.x = -1; offset.x <= 1; offset.x++ )
                {
                    const kl::Int2 offset_point = point + offset;
                    if ( !image_copy.in_bounds( offset_point ) || image_copy[offset_point] != index_color )
                    {
                        image[point] = BORDER_COLOR;
                        return;
                    }
                }
            }
            image[point] = kl::RGB{};
        } );
    image.save_to_file( path, kl::ImageType::PNG );
    log_success( "Generated border map" );
}

void generate_indices_map( std::vector<Country> const& countries, std::string_view const& path )
{
    kl::Image image;
    image.resize( { 8192, 4096 } );
    draw_indices( countries, image );
    image.save_to_file( path, kl::ImageType::PNG );
    log_success( "Generated indices map" );
}
