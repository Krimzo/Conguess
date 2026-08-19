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
    m_play_count = 0;
    m_player_score = 0;
    gen_possible();
    log( "Game Reset." );
    new_random_country();
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

void ConguessGame::gen_possible()
{
    auto& country_data = conguess.country_data;
    m_possible_countries.clear();
    for ( int i = 0; i < country_data.countries.size(); i++ )
    {
        auto& country = country_data.countries[i];
        if ( country.max_poly_area < min_allowed_poly_area )
            continue;
        m_possible_countries.emplace_back( i + 1 );
    }
}

void ConguessGame::new_random_country()
{
    if ( m_possible_countries.empty() )
    {
        this->reset();
        return;
    }
    m_fail_count = 0;
    const int rand_index = kl::random::gen_int( (int) m_possible_countries.size() );
    m_random_country = m_possible_countries[rand_index];
    m_possible_countries.erase( m_possible_countries.begin() + rand_index );
    log( "New Random Country Is: ", conguess.country_data.countries[(size_t) m_random_country - 1].name );
}
