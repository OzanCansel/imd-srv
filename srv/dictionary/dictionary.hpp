#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <string_view>

class dictionary
{

public:

    using map_t = std::unordered_map<std::string , std::string>;

    void        assign( const std::string& key , const std::string& val );
    void        remove( const std::string& key );
    std::string fetch( const std::string& key ) const;

private:

    mutable
    std::shared_timed_mutex m_mtx;
    map_t m_kv;
};