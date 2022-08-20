#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/strand.hpp>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <string>
#include "dictionary/dictionary.hpp"

class connection : public boost::enable_shared_from_this<connection>
{

public:

    using pointer    = boost::shared_ptr<connection>;
    using io_context = boost::asio::io_context;
    using socket     = boost::asio::ip::tcp::socket;
    using strand     = boost::asio::io_context::strand;
    using streambuf  = boost::asio::streambuf;

    static pointer create( io_context& , dictionary& );

    socket& sck();
    void    start();

private:

    connection( io_context& , dictionary& );

    void read_line();
    void handle_read_line( const boost::system::error_code& );
    void send_success( std::string req_id );
    void send_success( std::string req_id , std::string value );
    void send_error( std::string req_id , std::string err_msg );
    void send_error( std::string err_msg );
    void send( std::string );
    void queue_message( std::string );
    void start_packet_send();
    void packet_send_done( const boost::system::error_code& );

    streambuf               m_buffer;
    socket                  m_sck;
    strand                  m_write_strand;
    dictionary&             m_dictionary;
    std::deque<std::string> m_send_packet_queue;
};