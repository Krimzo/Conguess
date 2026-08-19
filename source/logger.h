#pragma once

#include "t.h"


struct Log
{
    std::string text;
    kl::RGB color;
    const uint64_t creation_time = kl::time::now();
};

struct Logger
{
    static inline constexpr kl::RGB LOG_COLOR = kl::colors::WHITE;
    static inline constexpr kl::RGB ERROR_COLOR = kl::colors::RED;
    static inline constexpr kl::RGB SUCCESS_COLOR = kl::colors::GREEN;

    int min_log_count = 10;
    float max_log_duration_s = 5.0f;

    Logger();

    void log( std::string_view const& text, kl::RGB color );

    void log( std::string_view const& text );
    void log_error( std::string_view const& text );
    void log_success( std::string_view const& text );

    int log_count() const;
    std::list<Log>::iterator get_iterator();

    int total_log_count() const;
    int total_error_count() const;
    int total_success_count() const;

    void remove_old();

private:
    std::list<Log> m_logs;
    int m_total_log_count = 0;
    int m_total_error_count = 0;
    int m_total_success_count = 0;
};

inline Logger LOGGER{};

template<typename... Args>
constexpr void log( Args&&... args )
{
    LOGGER.log( kl::format( args... ) );
}

template<typename... Args>
constexpr void log_error( Args&&... args )
{
    LOGGER.log_error( kl::format( args... ) );
}

template<typename... Args>
constexpr void log_success( Args&&... args )
{
    LOGGER.log_success( kl::format( args... ) );
}
