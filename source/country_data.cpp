#include "conguess.h"


bool tPolygon::contains( kl::Float2 point ) const
{
    bool in = false;
    for ( uint64_t i = 0, j = coords.size() - 1; i < coords.size(); j = i++ )
    {
        const bool left = ( coords[i].y > point.y ) != ( coords[j].y > point.y );
        const bool right = point.x < ( coords[j].x - coords[i].x ) * ( point.y - coords[i].y ) / ( coords[j].y - coords[i].y ) + coords[i].x;
        if ( left && right )
            in = !in;
    }
    return in;
}

kl::Int2 coords_to_point( kl::Int2 image_size, kl::Float2 coords );
kl::Float2 point_to_coords( kl::Int2 image_size, kl::Int2 point );
kl::Float4 min_max_coords( tPolygon const& polygon );
kl::RGB int_to4_value_color( int value );
void read_country_data( std::string_view const& path, std::vector<Country>& out_countries );
void generate_boundary_map( std::vector<Country> const& countries, std::string_view const& path );
void generate_indices_map( std::vector<Country> const& countries, std::string_view const& path );

ConguessCountryData::ConguessCountryData( Conguess& conguess )
    : conguess( conguess )
{
    static constexpr int MAX_COUNTRY_COUNT = 255;
    static constexpr std::string_view BOUNDARIES_MAP_OUT_PATH = "textures/earth_boundaries.png";
    static constexpr std::string_view INDICES_MAP_OUT_PATH = "textures/earth_indices.png";

    read_country_data( "data/countries.json", countries );
    const size_t country_count = conguess.country_data.countries.size();
    if ( country_count > MAX_COUNTRY_COUNT )
    {
        log_error( "Error. Max country count is ", MAX_COUNTRY_COUNT, " but ", country_count, " were provided." );
        return;
    }

    if ( !std::filesystem::exists( BOUNDARIES_MAP_OUT_PATH ) )
        generate_boundary_map( countries, BOUNDARIES_MAP_OUT_PATH );
    else
        log( "Boundaries map already exists ", BOUNDARIES_MAP_OUT_PATH );

    if ( !std::filesystem::exists( INDICES_MAP_OUT_PATH ) )
        generate_indices_map( countries, INDICES_MAP_OUT_PATH );
    else
        log( "Indices map already exists ", INDICES_MAP_OUT_PATH );
}

std::string const& ConguessCountryData::get_name( int index ) const
{
    return countries[index].name;
}

kl::Int2 coords_to_point( kl::Int2 image_size, kl::Float2 coords )
{
    kl::Int2 res;
    res.x = int( ( coords.y + 180.0f ) / 360.0f * image_size.x );
    res.y = image_size.y - int( ( coords.x + 90.0f ) / 180.0f * image_size.y );
    return res;
}

kl::Float2 point_to_coords( kl::Int2 image_size, kl::Int2 point )
{
    kl::Float2 res;
    res.x = ( image_size.y - point.y ) / (float) image_size.y * 180.0f - 90.0f;
    res.y = point.x / (float) image_size.x * 360.0f - 180.0f;
    return res;
}

kl::Float4 min_max_coords( tPolygon const& polygon )
{
    kl::Float4 res = { -1e3f, 1e3f, 1e3f, -1e3f };
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
                        ( *coord_cont_array )[0]->get_float().value_or( {} )
                    );
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

void generate_boundary_map( std::vector<Country> const& countries, std::string_view const& path )
{
    kl::Image image;
    image.resize( { 8192, 4096 } );
    const auto draw_country_boundaries = [&]( std::vector<tPolygon> const& polygons )
        {
            for ( auto const& [coords] : polygons )
            {
                kl::Float2 last_coord = coords.back();
                for ( auto const& coord : coords )
                {
                    const kl::Int2 start_pos = coords_to_point( image.size(), last_coord );
                    const kl::Int2 end_pos = coords_to_point( image.size(), coord );
                    for ( int i = 0; i < 2; i++ )
                        image.draw_line( start_pos + kl::Int2( i, i ), end_pos + kl::Int2( i, i ), kl::colors::WHITE );
                    last_coord = coord;
                }
            }
        };
    for ( uint64_t i = 0; i < countries.size(); i++ )
    {
        draw_country_boundaries( countries[i].polygons );
        log( "Generated boundary map ", i + 1, "/", countries.size(), " (", countries[i].name, ")" );
    }
    image.save_to_file( path, kl::ImageType::PNG );
    log_success( "Generated boundaries map" );
}

void generate_indices_map( std::vector<Country> const& countries, std::string_view const& path )
{
    kl::Image image;
    image.resize( { 8192, 4096 } );
    const auto draw_country_indices = [&]( std::vector<tPolygon> const& polygons, int index )
        {
            const kl::RGB index_color = { byte( index + 1 ), 0, 0 };
            for ( auto const& polygon : polygons )
            {
                const kl::Float4 square_bounds = min_max_coords( polygon );
                const kl::Int2 top_left = coords_to_point( image.size(), kl::Float2{ square_bounds.x, square_bounds.y } );
                const kl::Int2 bottom_right = coords_to_point( image.size(), kl::Float2{ square_bounds.z, square_bounds.w } );
                for ( kl::Int2 point = top_left; point.y <= bottom_right.y; point.y++ )
                {
                    for ( point.x = top_left.x; point.x <= bottom_right.x; point.x++ )
                    {
                        if ( image.in_bounds( point ) && polygon.contains( point_to_coords( image.size(), point ) ) )
                            image[point] = index_color;
                    }
                }
            }
        };
    std::atomic<int> map_counter = 0;
    const int countries_count = (int) countries.size();
    kl::async_for( 0, countries_count, [&]( int i )
        {
            draw_country_indices( countries[i].polygons, i );
            log( "Generated index map ", ++map_counter, "/", countries_count, " (", countries[i].name, ")" );
        } );
    image.save_to_file( path, kl::ImageType::PNG );
    kl::print( "Generated indices map" );
}
