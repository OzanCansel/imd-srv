#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <string_view>
#include <boost/optional.hpp>

class dictionary
{

public:

    using map_t        = std::unordered_map<std::string , std::string>;
    using nullable_str = boost::optional<std::string>;

    void         put( const std::string& key , const std::string& val );
    void         remove( const std::string& key );
    nullable_str get( const std::string& key ) const;

private:

    mutable std::shared_timed_mutex m_mtx;
    map_t m_kv;
};