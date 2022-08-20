#include "connection.hpp"
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <sstream>

static constexpr auto INAPPROPRIATE_COMMAND = "INAPPROPRIATE_COMMAND";
static constexpr auto NO_SUCH_A_KEY         = "NO_SUCH_A_KEY";
static constexpr auto MISSING_REQ_ID        = "MISSING_REQ_ID";
static constexpr auto MISSING_KEY           = "MISSING_KEY";
static constexpr auto MISSING_VALUE         = "MISSING_VALUE";

connection::pointer connection::create( io_context& ctx , dictionary& dict )
{
    return pointer(
        new connection { ctx , dict }
    );
}

connection::connection( io_context& ctx , dictionary& dict )
    :   m_sck { ctx }
    ,   m_dictionary { dict }
    ,   m_write_strand { ctx }
{}

connection::socket& connection::sck()
{
    return m_sck;
}

void connection::start()
{
    read_line();
}

void connection::read_line()
{
    boost::asio::async_read_until(
        m_sck ,
        m_buffer ,
        '\n' ,
        [ me = shared_from_this() ]
        ( boost::system::error_code err , int){
            me->handle_read_line( err );
        }
    );
}

void connection::handle_read_line( const boost::system::error_code& err )
{
    if ( err )
        return;

    std::istream ts( &m_buffer );
    std::string line;
    std::getline( ts , line );

    std::stringstream is { line };

    std::string cmd;

    is >> cmd;

    if ( cmd == "put" )
    {
        std::string req_id , key;

        is >> req_id >> key;

        std::string value;
        getline( is , value );
        boost::trim_left( value );

        if ( !req_id.empty() && !key.empty() && !value.empty() )
        {
            m_dictionary.put( key , value );

            send_success( move( req_id ) );
        }
        else if ( req_id.empty() )
        {
            send_error( MISSING_REQ_ID );
        }
        else if ( key.empty() )
        {
            send_error( move( req_id ) , MISSING_KEY );
        }
        else if ( value.empty() )
        {
            send_error( move( req_id ) , MISSING_VALUE );
        }
    }
    else if ( cmd == "remove" )
    {
        std::string req_id , key;

        is >> req_id >> key;

        if ( !req_id.empty() && !key.empty() )
        {
            m_dictionary.remove( key );
            send_success( move( req_id ) );
        }
        else if ( req_id.empty() )
        {
            send_error( MISSING_REQ_ID );
        }
        else if ( key.empty() )
        {
            send_error( move( req_id ) , MISSING_KEY );
        }
    }
    else if ( cmd == "get" )
    {
        std::string req_id , key;

        is >> req_id >> key;

        if ( !req_id.empty() && !key.empty() )
        {
            std::string response;
            dictionary::nullable_str value { m_dictionary.get( key ) };

            if ( !value.is_initialized() )
                send_error( move( req_id ) , NO_SUCH_A_KEY );
            else
                send_success( move( req_id ) , *value );
        }
        else if ( req_id.empty() )
        {
            send_error( MISSING_REQ_ID );
        }
        else if ( key.empty() )
        {
            send_error( move( req_id ) , MISSING_KEY );
        }
    }
    else
    {
        send_error( INAPPROPRIATE_COMMAND );
    }

    read_line();
}

void connection::send_success( std::string req_id )
{
    send(
        (
            boost::format( "%1% success\n" ) % req_id
        ).str()
    );
}

void connection::send_success( std::string req_id , std::string value )
{
    send(
        (
            boost::format( "%1% success %2%\n" ) % req_id % value
        ).str()
    );
}

void connection::send_error( std::string req_id , std::string err_msg )
{
    send(
        (
            boost::format( "%1% error %2%\n" ) % req_id % err_msg
        ).str()
    );
}

void connection::send_error( std::string err_msg )
{
    send(
        (
            boost::format( "error error %1%\n" ) % err_msg
        ).str()
    );
}

void connection::send( std::string message )
{
    m_write_strand.post(
        [ me = shared_from_this() , message = move( message ) ](){
            me->queue_message( message );
        }
    );
}

void connection::queue_message( std::string msg )
{
    bool write_in_progress = !m_send_packet_queue.empty();

    m_send_packet_queue.push_back( move( msg ) );

    if ( !write_in_progress )
        start_packet_send();
}

void connection::start_packet_send()
{
    boost::asio::async_write(
        m_sck ,
        boost::asio::buffer( m_send_packet_queue.front() ) ,
        m_write_strand.wrap(
            [ me = shared_from_this() ]( const boost::system::error_code& err , std::size_t ){
                me->packet_send_done( err );
            }
        )
    );
}

void connection::packet_send_done( const boost::system::error_code& err )
{
    if ( err )
        return;

    m_send_packet_queue.pop_front();

    if ( !m_send_packet_queue.empty() )
        start_packet_send();
}