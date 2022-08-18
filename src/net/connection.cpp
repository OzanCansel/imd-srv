#include "connection.hpp"
#include <boost/asio/read_until.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <string>
#include <iostream>
#include <iterator>

connection::pointer connection::create( io_context& ctx , dictionary& dict )
{
    return pointer(
        new connection { ctx , dict }
    );
}

connection::connection( io_context& ctx , dictionary& dict )
    :   m_sck { ctx }
    ,   m_dictionary { dict }
{}

connection::socket& connection::sck()
{
    return m_sck;
}

void connection::start()
{
    boost::asio::async_read_until(
        m_sck ,
        m_buffer ,
        '\n' ,
        boost::bind(
            &connection::process ,
            shared_from_this() ,
            boost::placeholders::_1 ,
            boost::placeholders::_2
        )
    );
}

void connection::process( boost::system::error_code ec , std::size_t bytes_read )
{
    if ( ec == boost::asio::error::eof )
    {
        print_disconnected();
    }
    else
    {
        parse();
    }

    start();
}

void connection::print_disconnected()
{
    auto endpoint = m_sck.remote_endpoint();

    std::cout << endpoint.address().to_string()
              << ':'
              << endpoint.port()
              << " disconnected.\n";
}

void connection::parse()
{
    std::istream is( &m_buffer );

    std::string cmd;

    is >> cmd;

    if ( cmd == "+" )
    {
        std::string req_id , key;

        is >> req_id >> key;

        std::string value;
        getline( is , value );
        boost::trim_left( value );

        if ( empty( req_id ) || empty( key ) || empty( value ) )
            return;

        m_dictionary.assign( key , value );
    }
    else if ( cmd == "-" )
    {
        std::string req_id , key;

        is >> req_id >> key;

        if ( empty( req_id ) || empty( key ) )
            return;

        m_dictionary.remove( key );
    }
    else if ( cmd == "?" )
    {
        std::string req_id , key;

        is >> req_id >> key;

        if ( empty( req_id ) || empty( key ) )
            return;

        std::string value { m_dictionary.fetch( key ) };
        std::string response {
            ( boost::format( "%1% %2%\n" ) % req_id % value ).str()
        };

        m_sck.async_write_some(
            boost::asio::buffer( response ) ,
            []( boost::system::error_code , std::size_t ){}
        );
    }
}