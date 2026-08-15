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
{
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

void ConguessInput::update()
{
    //static int last_scroll = 0;
    //const int scroll_delta = last_scroll - window->mouse.scroll();
    //camera.field_of_view = kl::math::minmax( camera.field_of_view + scroll_delta * 5.0f, 5.0f, 90.0f );
    //last_scroll = window->mouse.scroll();

    //kl::float3 mouse_sphere_intersect = {};
    //get_mouse_ray().intersect_sphere( sphere, mouse_sphere_intersect );

    //const auto mouse_sphere_intersect_no_y = kl::float3( mouse_sphere_intersect.x, 0.0f, mouse_sphere_intersect.z );
    //mouse_geo_location.x = mouse_sphere_intersect.angle( mouse_sphere_intersect_no_y );
    //mouse_geo_location.x *= ( mouse_sphere_intersect.y < 0.0f ) ? -1.0f : 1.0f;

    //const kl::float3 greenwich = ( kl::mat4::rotation( sphere_rotation ) * kl::float4( 1.0f, 0.0f, 0.0f, 1.0f ) ).xyz;
    //mouse_geo_location.y = kl::float2( greenwich.x, greenwich.z ).angle( kl::float2( mouse_sphere_intersect_no_y.x, mouse_sphere_intersect_no_y.z ), true );

    //for ( int i = 0; i < data::countries.size(); i++ )
    //{
    //    for ( auto& polygon : data::countries[i].polygons )
    //    {
    //        if ( polygon.contains( mouse_geo_location ) )
    //        {
    //            mouse_country_index = i;
    //            return;
    //        }
    //    }
    //}
    //mouse_country_index = -10;
}

void ConguessInput::new_random_country()
{
    const int old_game_country = conguess.game_random_country;
    while ( conguess.game_random_country == old_game_country )
        conguess.game_random_country = kl::random::gen_int( (int) conguess.country_data.countries.size() );
}
