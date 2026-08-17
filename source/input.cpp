#include "conguess.h"


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

    camera_distance -= mouse.scroll() * SCROLL_MULTI;
    camera_distance = kl::max( camera_distance, MIN_CAMERA_DISTANCE );

    const auto save_originals = [&]
        {
            original_rotations = {
                camera_rotation.x,
                rotate_camera_y ? camera_rotation.y : earth_rotation.y,
            };
            original_mouse_pos = mouse.position();
        };

    if ( mouse.right.pressed() && !mouse.middle.pressed() && !mouse.middle )
        save_originals();
    if ( mouse.right )
    {
        const kl::Int2 mouse_delta = mouse.position() - original_mouse_pos;
        const kl::Float2 rotation_delta = ANGLE_MULTI * mouse_delta;
        auto& rotation_x = camera_rotation.x;
        auto& rotation_y = rotate_camera_y ? camera_rotation.y : earth_rotation.y;
        rotation_x = kl::clamp( original_rotations.x + rotation_delta.y, -VERTICAL_ANGLE_LIMIT, VERTICAL_ANGLE_LIMIT );
        rotation_y = original_rotations.y + ( rotate_camera_y ? rotation_delta.x : -rotation_delta.x );
        if ( rotation_x == -VERTICAL_ANGLE_LIMIT || rotation_x == VERTICAL_ANGLE_LIMIT )
            save_originals();
    }

    mouse_geo_location.reset();
    mouse_country_index.reset();

    kl::Float3 mouse_earth_intersect;
    const kl::Ray mouse_ray{ camera.position, kl::inverse( camera.matrix() ), mouse.ndc_pos() };
    if ( mouse_ray.intersect_sphere( EARTH_SPHERE, &mouse_earth_intersect ) )
    {
        auto& mouse_geo_location = this->mouse_geo_location.emplace();

        mouse_geo_location.x = kl::angle( mouse_earth_intersect, { mouse_earth_intersect.x, 0.0f, mouse_earth_intersect.z } );
        mouse_geo_location.x *= mouse_earth_intersect.y < 0.0f ? -1.0f : 1.0f;

        const kl::Float3 greenwich = ( kl::Float4x4::rotation( earth_rotation ) * kl::Float4{ 0.0f, 0.0f, 1.0f, 1.0f } ).xyz();
        mouse_geo_location.y = kl::angle( kl::Float2{ greenwich.x, greenwich.z }, kl::Float2{ mouse_earth_intersect.x, mouse_earth_intersect.z }, true );
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

    const kl::Float4 back_vector = { 0.0f, 0.0f, -camera_distance, 1.0f };
    camera.position = ( kl::Float4x4::rotation( camera_rotation ) * back_vector ).xyz();
    camera.set_forward( -camera.position );

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
}
