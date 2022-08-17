#include "server.hpp"
#include <cstdint>
#include <iostream>

server::server( io_context& ctx , int port )
    :   m_ctx { ctx }
    ,   m_acceptor
        {
            ctx ,
            boost::asio::ip::tcp::endpoint
            {
                boost::asio::ip::tcp::v4() ,
                std::uint16_t( port )
            }
        }
{
    accept();
}

void server::accept()
{
    auto candidate = connection::create( m_ctx );

    m_acceptor.async_accept(
        candidate->sck() ,
        [ this , candidate = std::move( candidate ) ]( const boost::system::error_code& err )
        {
            auto endpoint = candidate->sck().remote_endpoint();

            std::cout << endpoint.address().to_string()
                      << ':'
                      << endpoint.port()
                      << " connected.\n";

            if ( !err )
                candidate->start();

            accept();
        }
    );
}