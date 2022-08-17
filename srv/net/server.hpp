#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include "connection.hpp"

class server
{

public:

    using io_context = boost::asio::io_context;
    using acceptor   = boost::asio::ip::tcp::acceptor;

    server( io_context& , int port );

private:

    void accept();
    void handle_accept( io_context& );

    io_context& m_ctx;
    acceptor    m_acceptor;
};