#include "conguess.h"


ConguessInput::ConguessInput( Conguess& conguess )
    :conguess( conguess )
{}

void ConguessInput::update()
{
    auto& window = conguess.window;
    auto& mouse = window.mouse;
    auto& keyboard = window.keyboard;
    auto& camera = conguess.camera;

    const auto save_originals = [&]
        {
            original_rotations = {
                camera_rotation.x,
                rotate_camera_y ? camera_rotation.y : earth_rotation.y,
            };
            original_mouse_pos = mouse.position();
        };

    mouse_country_index.reset();

    const kl::Ray mouse_ray{ camera.position, kl::inverse( camera.matrix() ), mouse.ndc_pos() };
    if ( const auto opt_inter = mouse_ray.intersect_sphere( EARTH_SPHERE ) )
    {
        auto const& mouse_earth_inter = *opt_inter;
        float2 mouse_geo_location;

        mouse_geo_location.x = kl::angle( mouse_earth_inter, { mouse_earth_inter.x, 0.0f, mouse_earth_inter.z } );
        mouse_geo_location.x *= mouse_earth_inter.y < 0.0f ? -1.0f : 1.0f;

        const float3 greenwich = ( float4x4::rotation( earth_rotation ) * float4{ 0.0f, 0.0f, 1.0f, 1.0f } ).xyz();
        mouse_geo_location.y = kl::angle( float2{ greenwich.x, greenwich.z }, float2{ mouse_earth_inter.x, mouse_earth_inter.z }, true );
        mouse_geo_location.y -= 180.0f;

        for ( int i = 0; i < (int) conguess.country_data.countries.size(); i++ )
        {
            auto const& country = conguess.country_data.countries[i];
            for ( auto const& polygon : country.polygons )
            {
                if ( polygon.contains( mouse_geo_location ) )
                {
                    mouse_country_index.emplace( i + 1 );
                    goto country_loop_end;
                }
            }
        }
    country_loop_end:;
    }

    if ( keyboard.a.pressed() )
        conguess.postprocess.render_atmosphere = !conguess.postprocess.render_atmosphere;
    if ( keyboard.b.pressed() )
        conguess.postprocess.render_borders = !conguess.postprocess.render_borders;
    if ( keyboard.c.pressed() )
        conguess.earth.render_clouds = !conguess.earth.render_clouds;
    if ( keyboard.r.pressed() )
        conguess.game.reset();
    if ( keyboard.m.pressed() )
    {
        rotate_camera_y = !rotate_camera_y;
        save_originals();
    }
    if ( keyboard.f11.pressed() )
        conguess.gpu.set_fullscreen( !conguess.gpu.fullscreened() );
    if ( keyboard.esc.pressed() )
        conguess.window.close();

    camera_distance -= mouse.scroll() * SCROLL_MULTI;
    camera_distance = kl::max( camera_distance, MIN_CAMERA_DISTANCE );
    const float camera_deg_per_px = calc_deg_per_px();

    if ( mouse.left.pressed() )
    {
        original_mouse_country = mouse_country_index.value_or( -1 );
        original_is_correct = conguess.game.is_correct( original_mouse_country );
        if ( mouse_country_index )
            conguess.game.play_country( *mouse_country_index );
    }
    if ( mouse.left )
    {
        if ( mouse_country_index == original_mouse_country )
            conguess.postprocess.hold_country_color_multi = original_is_correct ? CORRECT_HOLD_COLOR : WRONG_HOLD_COLOR;
        else
            conguess.postprocess.hold_country_color_multi = NEUTRAL_HOLD_COLOR;
    }

    if ( mouse.right.pressed() )
        save_originals();
    if ( mouse.right )
    {
        const float2 mouse_delta = mouse.position() - original_mouse_pos;
        const float2 rotation_delta = {
            mouse_delta.y * camera_deg_per_px,
            mouse_delta.x * camera_deg_per_px };
        auto& rotation_x = camera_rotation.x;
        auto& rotation_y = rotate_camera_y ? camera_rotation.y : earth_rotation.y;
        rotation_x = kl::clamp( original_rotations.x + rotation_delta.x, -VERTICAL_ANGLE_LIMIT, VERTICAL_ANGLE_LIMIT );
        rotation_y = original_rotations.y + ( rotate_camera_y ? rotation_delta.y : -rotation_delta.y );
        if ( rotation_x == -VERTICAL_ANGLE_LIMIT || rotation_x == VERTICAL_ANGLE_LIMIT )
            save_originals();
    }

    const float4 back_vector = { 0.0f, 0.0f, -camera_distance, 1.0f };
    camera.position = ( float4x4::rotation( camera_rotation ) * back_vector ).xyz();
    camera.set_forward( -camera.position );
}

float ConguessInput::calc_deg_per_px() const
{
    constexpr float2 p1 = { 1.0f, 0.0f };
    constexpr float2 p2 = { TESTED_CAM_DIST, ANGLE_MULTI };
    return kl::line_y( p1, p2, camera_distance );
}
