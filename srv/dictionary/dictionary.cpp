#include "dictionary.hpp"
#include <mutex>

void dictionary::assign( const std::string& key , const std::string& value )
{
    std::unique_lock<std::shared_timed_mutex> w( m_mtx );

    m_kv.insert( { key , value } );
}

void dictionary::remove( const std::string& key )
{
    std::unique_lock<std::shared_timed_mutex> w( m_mtx );

    m_kv.erase( key );
}

std::string dictionary::fetch( const std::string& key ) const
{
    std::shared_lock<std::shared_timed_mutex> r( m_mtx );

    auto it = m_kv.find( key );

    if ( it == end( m_kv ) )
        return std::string {};
    else
        return it->second;
}