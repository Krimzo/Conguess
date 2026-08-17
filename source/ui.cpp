#include "conguess.h"


ConguessUI::ConguessUI( Conguess& conguess )
    : conguess( conguess )
{}

void ConguessUI::update()
{
    auto& window = conguess.window;
    auto& gpu = conguess.gpu;

    LOGGER.remove_old();

    font_size = window.pixels_to_dips( window.height() / 50.0f );
    if ( !text_format || text_format->GetFontSize() != font_size )
        text_format = gpu.create_text_format( L"roboto",
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            font_size );

    gpu.text_batch.resize( LOGGER.log_count() );
    auto it = LOGGER.get_iterator();
    for ( int i = 0; i < (int) gpu.text_batch.size(); i++ )
    {
        auto& log = *( it++ );
        auto& text = gpu.text_batch[i];
        text.format = text_format;
        text.color = log.color;
        text.position = { -0.95f, 0.9f };
        text.rect_size = kl::Float2{
            ( 0.0f ) - text.position.x,
            text.position.y - ( -1.0f ) };
        text.data = std::wstring( i, L'\n' ) + kl::convert_string( log.text );
        text.hor_center = false;
        text.ver_center = false;
    }

    gpu.draw_text_batch();
}
