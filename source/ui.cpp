#include "conguess.h"


struct Keybind
{
    std::string key;
    std::string description;
    bool is_toggle = false;
    bool state = false;
};

ConguessUI::ConguessUI( Conguess& conguess )
    : conguess( conguess )
{}

void ConguessUI::update()
{
    auto& window = conguess.window;
    auto& gpu = conguess.gpu;
    auto& input = conguess.input;
    auto& earth = conguess.earth;
    auto& postprocess = conguess.postprocess;

    LOGGER.remove_old();
    gpu.raster_batch.clear();

    log_font_size = window.pixels_to_dips( window.height() / 60.0f );
    if ( !log_text_format || log_text_format->GetFontSize() != log_font_size )
    {
        log_text_format = gpu.create_text_format( L"roboto",
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            log_font_size );
    }

    auto it = LOGGER.get_iterator();
    for ( int i = 0; i < LOGGER.log_count(); i++ )
    {
        auto& log = *( it++ );
        kl::Ref<kl::RasterText> text = new kl::RasterText();
        text->format = log_text_format;
        text->data = std::wstring( i, L'\n' ) + kl::convert_string( log.text );
        text->color = log.color;
        text->box_top_left = { -0.9f, 0.9f };
        text->box_bottom_right = { -0.1f, -0.9f };
        text->h_align = kl::HAlign::DWRITE_TEXT_ALIGNMENT_LEADING;
        text->v_align = kl::VAlign::DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        gpu.raster_batch.push_back( text );
    }

    std::vector<Keybind> keybinds;
    keybinds.push_back( Keybind{ .key = "a", .description = "Toggle Atmosphere", .is_toggle = true, .state = postprocess.render_atmosphere } );
    keybinds.push_back( Keybind{ .key = "b", .description = "Toggle Borders", .is_toggle = true, .state = postprocess.render_borders } );
    keybinds.push_back( Keybind{ .key = "c", .description = "Toggle Clouds", .is_toggle = true, .state = earth.render_clouds } );
    keybinds.push_back( Keybind{ .key = "r", .description = "Reset Game", .is_toggle = false } );
    keybinds.push_back( Keybind{ .key = "m", .description = "Toggle Camera Rotation", .is_toggle = true, .state = input.rotate_camera_y } );
    keybinds.push_back( Keybind{ .key = "f11", .description = "Toggle Fullscreen", .is_toggle = false } );
    keybinds.push_back( Keybind{ .key = "esc", .description = "Exit Game", .is_toggle = false } );

    keybind_font_size = window.pixels_to_dips( window.height() / 80.0f );
    if ( !keybind_text_format || keybind_text_format->GetFontSize() != keybind_font_size )
    {
        keybind_text_format = gpu.create_text_format( L"roboto",
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            keybind_font_size );
    }

    int line_count = 0;
    for ( auto& keybind : keybinds )
    {
        kl::Ref<kl::RasterText> text = new kl::RasterText();
        text->format = keybind_text_format;
        text->data = std::wstring( line_count++, L'\n' ) + kl::wformat( keybind.key.c_str(), ": ", keybind.description.c_str() );
        text->color = keybind.is_toggle ? ( keybind.state ? kl::colors::GREEN : kl::colors::RED ) : ( kl::colors::WHITE );
        text->box_top_left = { 0.1f, 0.9f };
        text->box_bottom_right = { 0.9f, -0.9f };
        text->h_align = kl::HAlign::DWRITE_TEXT_ALIGNMENT_TRAILING;
        text->v_align = kl::VAlign::DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
        gpu.raster_batch.push_back( text );
    }

    gpu.draw_raster_batch();
}
