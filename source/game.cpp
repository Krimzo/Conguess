#include "conguess.h"


ConguessGame::ConguessGame( Conguess& conguess )
    :conguess( conguess )
{}

void ConguessGame::play_country( int index )
{
    auto& country_data = conguess.country_data;
    m_play_count += 1;
    if ( !is_correct( index ) )
    {
        conguess.postprocess.hold_country_color_multi = WRONG_HOLD_COLOR;
        log_error( "Incorrect, that was: ", country_data.get_name( index - 1 ) );
        return;
    }
    m_player_score += 1;
    conguess.postprocess.hold_country_color_multi = CORRECT_HOLD_COLOR;
    log_success( "Correct, player score: ", m_player_score, " / ", m_play_count );
    new_random_country();
}

void ConguessGame::reset()
{
    log( "Game Reset." );
    new_random_country();
    m_play_count = 0;
    m_player_score = 0;
}

int ConguessGame::play_count() const
{
    return m_play_count;
}

int ConguessGame::player_score() const
{
    return m_player_score;
}

bool ConguessGame::is_correct( int index ) const
{
    return index == m_random_country;
}

void ConguessGame::new_random_country()
{
    auto& country_data = conguess.country_data;
    const int old_random_country = m_random_country;
    while ( m_random_country == old_random_country )
        m_random_country = kl::random::gen_int( (int) country_data.countries.size() ) + 1;
    log( "New Random Country Is: ", country_data.get_name( m_random_country - 1 ) );
}
