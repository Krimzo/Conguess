#include "conguess.h"


void load_box_texture( kl::GPU& gpu, std::string_view const& path, kl::dx::ShaderView& out_sv );

ConguessSkybox::ConguessSkybox( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    mesh = gpu.create_cube_mesh( 1.0f );
    load_shaders( gpu, "skybox", shaders );
    load_box_texture( gpu, "textures/stars_milky.jpg", texture );
    sampler = gpu.create_sampler_state( true, false );
}

void ConguessSkybox::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_target_depth_view( conguess.render_target_view, {} );

    gpu.bind_shader_view_for_pixel_shader( texture, 0 );
    gpu.bind_sampler_state_for_pixel_shader( sampler, 0 );

    struct alignas( 16 ) CB
    {
        kl::Float4x4 VP;
    } cb = {};

    cb.VP = conguess.camera.matrix();

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );
}

void load_box_texture( kl::GPU& gpu, std::string_view const& path, kl::dx::ShaderView& out_sv )
{
    log( "Loading cube image ", path );
    const kl::Image image{ path };
    if ( image.pixel_count() == 0 )
    {
        log_error( "Failed to load cube image ", path );
        return;
    }
    if ( image.width() % 4 != 0 || image.height() % 3 != 0 )
    {
        log_error( "Cube image ", path, " has unsupported ratio" );
        return;
    }

    const int part_width = image.width() / 4;
    const int part_height = image.height() / 3;
    if ( part_width != part_height )
    {
        log_error( "Cube image ", path, " width part and height part are not the same" );
        return;
    }

    const kl::Int2 part_size{ part_width, part_height };
    const kl::Image front = image.rectangle( part_size * kl::Int2( 1, 1 ), part_size * kl::Int2( 2, 2 ) );
    const kl::Image back = image.rectangle( part_size * kl::Int2( 3, 1 ), part_size * kl::Int2( 4, 2 ) );
    const kl::Image left = image.rectangle( part_size * kl::Int2( 0, 1 ), part_size * kl::Int2( 1, 2 ) );
    const kl::Image right = image.rectangle( part_size * kl::Int2( 2, 1 ), part_size * kl::Int2( 3, 2 ) );
    const kl::Image top = image.rectangle( part_size * kl::Int2( 1, 0 ), part_size * kl::Int2( 2, 1 ) );
    const kl::Image bottom = image.rectangle( part_size * kl::Int2( 1, 2 ), part_size * kl::Int2( 2, 3 ) );
    log( "Loading cube texture ", path );
    const kl::dx::Texture cube_texture = gpu.create_cube_texture( front, back, left, right, top, bottom );
    if ( !cube_texture )
    {
        log_error( "Failed to load cube texture ", path );
        return;
    }
    log( "Creating cube shader view ", path );
    out_sv = gpu.create_shader_view( cube_texture, {} );
    if ( !out_sv )
    {
        log_error( "Failed to create cube shader view ", path );
        return;
    }
    log_success( "Cube texture ", path, " is good." );
}
