#include "conguess.h"


static kl::dx::ShaderView process_box_image( kl::GPU& gpu, kl::Image const& image )
{
    if ( image.width() % 4 != 0 || image.height() % 3 != 0 )
    {
        assert( false && "Texture has unsupported ratio!" );
        return {};
    }

    const int part_width = image.width() / 4;
    const int part_height = image.height() / 3;
    if ( part_width != part_height )
        return {};

    const kl::Int2 part_size{ part_width, part_height };
    const kl::Image front = image.rectangle( part_size * kl::Int2( 1, 1 ), part_size * kl::Int2( 2, 2 ) );
    const kl::Image back = image.rectangle( part_size * kl::Int2( 3, 1 ), part_size * kl::Int2( 4, 2 ) );
    const kl::Image left = image.rectangle( part_size * kl::Int2( 0, 1 ), part_size * kl::Int2( 1, 2 ) );
    const kl::Image right = image.rectangle( part_size * kl::Int2( 2, 1 ), part_size * kl::Int2( 3, 2 ) );
    const kl::Image top = image.rectangle( part_size * kl::Int2( 1, 0 ), part_size * kl::Int2( 2, 1 ) );
    const kl::Image bottom = image.rectangle( part_size * kl::Int2( 1, 2 ), part_size * kl::Int2( 2, 3 ) );
    return gpu.create_shader_view( gpu.create_cube_texture( front, back, left, right, top, bottom ), {} );
}

ConguessSkybox::ConguessSkybox( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    depth_state = gpu.create_depth_state( false, false, false );

    log( "Compiling skybox shaders" );
    shaders = gpu.create_shaders( kl::read_file_string( "shaders/skybox.hlsl" ) );

    log( "Loading box mesh" );
    mesh = gpu.create_vertex_buffer( (std::string_view) "meshes/cube.obj", true );

    log( "Loading skybox texture" );
    texture = process_box_image( gpu, kl::Image( "textures/stars_milky.png" ) );
}

void ConguessSkybox::update()
{
    auto& gpu = conguess.gpu;

    gpu.clear_target_view( conguess.render_target_view, kl::colors::GRAY );

    gpu.bind_target_depth_view( conguess.render_target_view, {} );
    gpu.bind_depth_state( depth_state );

    shaders.upload( conguess.camera.matrix() );
    gpu.bind_shaders( shaders );
    gpu.bind_shader_view_for_pixel_shader( texture, 0 );

    gpu.draw( mesh );
}
