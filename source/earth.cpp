#include "conguess.h"


ConguessEarth::ConguessEarth( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    depth_state = gpu.create_depth_state( true, false, false );

    log( "Compiling earth shaders" );
    shaders = gpu.create_shaders( kl::read_file_string( "shaders/earth.hlsl" ) );

    log( "Loading box mesh" );
    mesh = gpu.create_vertex_buffer( (std::string_view) "meshes/sphere.obj", true );

    log( "Loading earth day texture" );
    earth_day_texture = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_day.png" } ), {} );

    log( "Loading earth night texture" );
    earth_night_texture = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_night.png" } ), {} );

    log( "Loading earth clouds texture" );
    earth_clouds_texture = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_clouds.png" } ), {} );

    log( "Loading earth normal map" );
    earth_normal_map = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_normal.png" } ), {} );

    log( "Loading earth roughness map" );
    earth_roughness_map = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_roughness.png" } ), {} );

    log( "Loading earth boundaries map" );
    earth_boundaries_map = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_boundaries.png" } ), {} );

    log( "Loading earth indices map" );
    earth_indices_map = gpu.create_shader_view( gpu.create_texture(
        kl::Image{ "textures/earth_indices.png" } ), {} );
}

void ConguessEarth::update()
{
    auto& gpu = conguess.gpu;

    struct vs_cb
    {
        kl::Float4x4 w_matrix{};
        kl::Float4x4 vp_matrix{};
    };

    struct ps_cb
    {
        kl::Float4 sun_direction{};
        kl::Float4 camera_position{};
        kl::Float4 misc_data{};
    };

    gpu.clear_target_view( conguess.index_target_view, {} );

    gpu.bind_target_depth_views( { conguess.render_target_view.get(), conguess.index_target_view.get() }, {} );
    gpu.bind_depth_state( depth_state );

    gpu.bind_shaders( shaders );
    gpu.bind_shader_view_for_pixel_shader( earth_day_texture, 0 );
    gpu.bind_shader_view_for_pixel_shader( earth_night_texture, 1 );
    gpu.bind_shader_view_for_pixel_shader( earth_clouds_texture, 2 );
    gpu.bind_shader_view_for_pixel_shader( earth_normal_map, 3 );
    gpu.bind_shader_view_for_pixel_shader( earth_roughness_map, 4 );
    gpu.bind_shader_view_for_pixel_shader( earth_boundaries_map, 5 );
    gpu.bind_shader_view_for_pixel_shader( earth_indices_map, 6 );

    vs_cb vs_data{};
    vs_data.w_matrix = kl::Float4x4::rotation( conguess.sphere_rotation );
    vs_data.vp_matrix = conguess.camera.matrix();
    gpu.set_vertex_const_buffer( vs_data );

    ps_cb ps_data{};
    ps_data.sun_direction = { kl::normalize( conguess.sun_direction ), 0.0f };
    ps_data.camera_position = { conguess.camera.position, 0.0f };
    ps_data.misc_data = {
        conguess.timer.elapsed() / 1800.0f,
        float( conguess.input.mouse_country_index + 1 ),
        (float) render_clouds,
        0.0f
    };
    gpu.set_pixel_const_buffer( ps_data );

    gpu.draw( mesh );

    for ( int i = 0; i <= 6; i++ )
        gpu.unbind_shader_view_for_pixel_shader( i );
}
