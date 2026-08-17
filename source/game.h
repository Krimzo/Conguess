#pragma once

#include "helper.h"


struct ConguessGame
{
    Conguess& conguess;

    double min_country_area = 20'000.0;
    int hightlight_at_fail_count = 3;

    ConguessGame( Conguess& conguess );

    void play_country( int index );
    void reset();

    int current_rand() const;
    int play_count() const;
    int fail_count() const;
    int player_score() const;

    bool is_correct( int index ) const;
    bool should_highlight() const;

private:
    int m_random_country = 0;
    int m_play_count = 0;
    int m_fail_count = 0;
    int m_player_score = 0;

    void new_random_country();
};
