#pragma once

#include "helper.h"


struct ConguessGame
{
    Conguess& conguess;

    ConguessGame( Conguess& conguess );

    void play_country( int index );
    void reset();

    int play_count() const;
    int player_score() const;

    bool is_correct( int index ) const;

private:
    int m_random_country = 0;
    int m_play_count = 0;
    int m_player_score = 0;

    void new_random_country();
};
