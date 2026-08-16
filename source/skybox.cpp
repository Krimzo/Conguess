#include "conguess.h"


void load_box_texture( kl::GPU& gpu, std::string_view const& path, kl::dx::ShaderView& out_sv );

ConguessSkybox::ConguessSkybox( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    mesh = gpu.create_cube_mesh( 1.0f );
    raster_state = gpu.create_raster_state( false, false );
    load_shaders( gpu, "skybox", shaders );
    load_box_texture( gpu, "textures/stars_milky.jpg", texture );
    sampler = gpu.create_sampler_state( true, false );
}

void ConguessSkybox::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_target_depth_view( conguess.render_target_view, {} );
    gpu.bind_raster_state( raster_state );

    gpu.bind_shader_view_for_pixel_shader( texture, 0 );
    gpu.bind_sampler_state_for_pixel_shader( sampler, 0 );

    struct alignas( 16 ) CB
    {
        kl::Float4x4 VP;
        kl::Float3 SUN_DIRECTION;
    } cb = {};

    cb.VP = conguess.camera.matrix();
    cb.SUN_DIRECTION = kl::normalize( conguess.earth.sun_direction );

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );
}

void load_box_texture( kl::GPU& gpu, std::string_view const& path, kl::dx::ShaderView& out_sv )
{
    log( "Loading cube image ", path );
    kl::Texture box_texture{ gpu };
    box_texture.image.load_from_file( path );
    if ( box_texture.image.pixel_count() == 0 )
    {
        log_error( "Failed to load cube image ", path );
        return;
    }
    if ( box_texture.image.width() % 4 != 0 || box_texture.image.height() % 3 != 0 )
    {
        log_error( "Cube image ", path, " has unsupported ratio" );
        return;
    }
    log( "Loading cube texture ", path );
    if ( !box_texture.reload_as_cube() )
    {
        log_error( "Failed to load cube texture ", path );
        return;
    }
    log( "Creating cube shader view ", path );
    box_texture.create_shader_view();
    if ( !box_texture.shader_view )
    {
        log_error( "Failed to create cube shader view ", path );
        return;
    }
    out_sv = box_texture.shader_view;
    log_success( "Cube texture ", path, " is good." );
}
