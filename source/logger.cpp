#include "logger.h"


Logger::Logger()
{}

void Logger::log( std::string_view const& text, kl::RGB color )
{
    kl::print( color, text, kl::colors::CONSOLE );
    m_logs.emplace_back( text.data(), color );
    m_total_log_count += 1;
}

void Logger::log( std::string_view const& text )
{
    log( text, LOG_COLOR );
}

void Logger::log_error( std::string_view const& text )
{
    log( text, ERROR_COLOR );
    m_total_error_count += 1;
}

void Logger::log_success( std::string_view const& text )
{
    log( text, SUCCESS_COLOR );
    m_total_success_count += 1;
}

int Logger::log_count() const
{
    return (int) m_logs.size();
}

std::list<Log>::iterator Logger::get_iterator()
{
    return m_logs.begin();
}

int Logger::total_log_count() const
{
    return m_total_log_count;
}

int Logger::total_error_count() const
{
    return m_total_error_count;
}

int Logger::total_success_count() const
{
    return m_total_success_count;
}

void Logger::remove_old()
{
    if ( m_logs.size() <= min_log_count )
        return;
    m_logs.remove_if( [this]( Log const& l ) -> bool
        {
            if ( m_logs.size() <= min_log_count )
                return false;
            return kl::time::elapsed( l.creation_time ) > max_log_duration_s;
        } );
}
