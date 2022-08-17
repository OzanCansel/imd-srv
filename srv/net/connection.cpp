#include "connection.hpp"
#include <boost/asio/read_until.hpp>
#include <boost/bind.hpp>
#include <string>
#include <iostream>

connection::pointer connection::create( io_context& ctx )
{
    return pointer(
        new connection { ctx }
    );
}

connection::connection( io_context& ctx )
    :   m_sck { ctx }
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
            &connection::parse ,
            shared_from_this() ,
            boost::placeholders::_1 ,
            boost::placeholders::_2
        )
    );
}

void connection::parse( boost::system::error_code ec , std::size_t bytes_read )
{
    if ( ec == boost::asio::error::eof )
    {
        print_disconnected();
    }
    else
    {
        std::string  line;
        std::istream is( &m_buffer );

        std::getline( is , line );

        std::cout << line << std::endl;
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