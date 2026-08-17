#include "conguess.h"
#include "game.h"


ConguessGame::ConguessGame( Conguess& conguess )
    :conguess( conguess )
{}

void ConguessGame::play_country( int index )
{
    auto& country_data = conguess.country_data;
    m_play_count += 1;
    if ( !is_correct( index ) )
    {
        m_fail_count += 1;
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

int ConguessGame::current_rand() const
{
    return m_random_country;
}

int ConguessGame::play_count() const
{
    return m_play_count;
}

int ConguessGame::fail_count() const
{
    return m_fail_count;
}

int ConguessGame::player_score() const
{
    return m_player_score;
}

bool ConguessGame::is_correct( int index ) const
{
    return index == m_random_country;
}

bool ConguessGame::should_highlight() const
{
    return m_fail_count >= hightlight_at_fail_count;
}

void ConguessGame::new_random_country()
{
    m_fail_count = 0;
    auto& countries = conguess.country_data.countries;
    const int old_random_country = m_random_country;
    do
    {
        m_random_country = kl::random::gen_int( (int) countries.size() ) + 1;
    }
    while ( m_random_country == old_random_country && countries[(size_t) m_random_country - 1].total_area < min_country_area );
    log( "New Random Country Is: ", countries[(size_t) m_random_country - 1].name );
}
