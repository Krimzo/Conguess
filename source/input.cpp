#include "conguess.h"


//static bool last_intersect = false;
//static kl::float3 last_direction = {};
//static const kl::sphere sphere = { {}, 1.0f };
//
//static kl::ray get_mouse_ray()
//{
//    const auto frame_size = kl::float2( window->size() );
//    const kl::float2 ndc = {
//        static_cast<float>( window->mouse.position().x ),
//        static_cast<float>( frame_size.y - window->mouse.position().y )
//    };
//    return { camera, ndc / frame_size * 2.0f - kl::float2( 1.0f, 1.0f ) };
//}
//
//static void save_last_values()
//{
//    last_intersect = get_mouse_ray().intersect_sphere( sphere, last_direction );
//    last_direction = last_direction.normalize();
//}

ConguessInput::ConguessInput( Conguess& conguess )
    :conguess( conguess )
{}

void ConguessInput::update()
{
    static constexpr float MIN_CAMERA_DISTANCE = 1.05f;
    static constexpr float VERTICAL_ANGLE_LIMIT = 85.0f;
    static constexpr float SCROLL_MULTI = 0.05f;
    static constexpr kl::Float2 ANGLE_MULTI{ 0.1f };
    static constexpr kl::Sphere EARTH_SPHERE{ {}, 1.0f };

    auto& window = conguess.window;
    auto& mouse = window.mouse;
    auto& keyboard = window.keyboard;
    auto& camera = conguess.camera;

    conguess.camera_distance -= mouse.scroll() * SCROLL_MULTI;
    conguess.camera_distance = kl::max( conguess.camera_distance, MIN_CAMERA_DISTANCE );

    if ( mouse.right.pressed() )
    {
        original_rotations = conguess.rotations;
        original_mouse_pos = mouse.position();
    }

    if ( mouse.right )
    {
        const kl::Int2 mouse_delta = mouse.position() - original_mouse_pos;
        const kl::Float2 rotation_delta = ANGLE_MULTI * mouse_delta;
        conguess.rotations = {
            original_rotations.x + rotation_delta.y,
            original_rotations.y + rotation_delta.x,
        };
        conguess.rotations.x = kl::clamp( conguess.rotations.x, -VERTICAL_ANGLE_LIMIT, VERTICAL_ANGLE_LIMIT );
    }

    kl::Float3 mouse_earth_intersect;
    const kl::Ray mouse_ray{ camera.position, kl::inverse( camera.matrix() ), mouse.ndc_pos() };
    mouse_ray.intersect_sphere( EARTH_SPHERE, &mouse_earth_intersect );

    mouse_geo_location.x = kl::angle( mouse_earth_intersect, { mouse_earth_intersect.x, 0.0f, mouse_earth_intersect.z } );
    mouse_geo_location.x *= mouse_earth_intersect.y < 0.0f ? -1.0f : 1.0f;

    const kl::Float3 greenwich = ( kl::Float4x4::rotation( { 0.0f, -conguess.rotations.y, 0.0f } ) * kl::Float4{ 1.0f, 0.0f, 0.0f, 1.0f } ).xyz();
    mouse_geo_location.y = kl::angle( kl::Float2{ greenwich.x, greenwich.z }, kl::Float2{ mouse_earth_intersect.x, mouse_earth_intersect.z }, false );
    mouse_geo_location.y *= mouse_earth_intersect.x < 0.0f ? -1.0f : 1.0f;

    mouse_country_index = -10;
    for ( int i = 0; i < (int) conguess.country_data.countries.size(); i++ )
    {
        auto const& country = conguess.country_data.countries[i];
        for ( auto const& polygon : country.polygons )
        {
            if ( polygon.contains( mouse_geo_location ) )
            {
                mouse_country_index = i;
                goto country_loop_end;
            }
        }
    }
country_loop_end:;

    kl::print( std::fixed, "Camera Position ", camera.position );
    kl::print( std::fixed, "Sphere Rotation ", kl::Float3{ 0.0f, -conguess.rotations.y, 0.0f } );
    kl::print( std::fixed, mouse_geo_location, " -> ", mouse_country_index );

    //// Mouse
    //window->mouse.left.on_press = [&]
    //    {
    //        save_last_values();
    //    };

    //window->mouse.left.on_down = [&]
    //    {
    //        if ( kl::float3 current_direction; last_intersect && get_mouse_ray().intersect_sphere( sphere, current_direction ) )
    //        {
    //            current_direction = current_direction.normalize();

    //            const kl::float2 delta_angles = {
    //                kl::float2( last_direction.z, last_direction.y ).angle( { current_direction.z, current_direction.y }, true ),
    //                kl::float2( last_direction.x, last_direction.z ).angle( { current_direction.x, current_direction.z }, true )
    //            };

    //            sphere_rotation.y -= delta_angles.y;

    //            const float camera_angle_x = kl::math::to_degrees( std::asin( camera.get_forward().y ) );
    //            const float new_camera_angle_x = kl::math::minmax( camera_angle_x - delta_angles.x, -85.0f, 85.0f );
    //            const float new_camera_angle_x_rads = kl::math::to_radians( new_camera_angle_x );
    //            camera.set_forward( { 0.0f, std::sin( new_camera_angle_x_rads ), std::cos( new_camera_angle_x_rads ) } );
    //            camera.position = camera.get_forward() * -2.0f;

    //            save_last_values();
    //        }
    //    };

    //window->mouse.left.on_release = [&]
    //    {
    //        if ( mouse_country_index == last_random_country )
    //        {
    //            new_random_country();
    //            player_score++;
    //        }
    //        else if ( mouse_country_index >= 0 )
    //        {
    //            player_score--;
    //        }
    //        log_play_stats();
    //    };

    //window->mouse.middle.on_press = window->mouse.left.on_press;
    //window->mouse.right.on_press = window->mouse.left.on_press;
    //window->mouse.middle.on_down = window->mouse.left.on_down;
    //window->mouse.right.on_down = window->mouse.left.on_down;

    //// Keyboard
    //window->keyboard.r.on_press = [&]()
    //    {
    //        new_random_country();
    //        player_score = 0;
    //        log_play_stats();
    //    };
    //window->keyboard.c.on_press = [&]()
    //    {
    //        render::render_clouds = !render::render_clouds;
    //    };
    //window->keyboard.b.on_press = [&]()
    //    {
    //        postprocess::render_bounds = !postprocess::render_bounds;
    //    };
}

void ConguessInput::new_random_country()
{
    const int old_game_country = conguess.game_random_country;
    while ( conguess.game_random_country == old_game_country )
        conguess.game_random_country = kl::random::gen_int( (int) conguess.country_data.countries.size() );
}
