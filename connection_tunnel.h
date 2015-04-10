
#ifndef SFERA_PROXY_CONNECTION_TUNNEL_H
#define SFERA_PROXY_CONNECTION_TUNNEL_H

//----------------------------------------------------------------------------------------------------------------------

#include "boost_asio_headers.h"

//----------------------------------------------------------------------------------------------------------------------

class connection_tunnel
{
public:
    typedef std::function<void(connection_tunnel*)> callback_t;

private:
    size_t _id;
    callback_t _connectionClosedCallback;

    unique_ptr<b_ip::tcp::socket> _clientSocket;
    unique_ptr<b_ip::tcp::socket> _serverSocket;

    b_asio::streambuf _clientBuff;
    b_asio::streambuf _serverBuff;

    bool _closing = false;
    unsigned _pendingAsyncOperations = 0;


private:
    void _onError(const b_sys::error_code &e, const std::string &location);
    void _handleConnectedToServer(const b_sys::error_code &ec);

    void _handleClientRead(const b_sys::error_code &ec, size_t bytesTransferred);
    void _handleServerWrite(const b_sys::error_code &ec, size_t bytesTransferred);
    void _handleClientWrite(const b_sys::error_code &ec, size_t bytesTransferred);
    void _handleServerRead(const b_sys::error_code &ec, size_t bytesTransferred);

    void _startAsyncClientRead();
    void _startAsyncServerWrite();
    void _startAsyncServerRead();
    void _startAsyncClientWrite();


public:
    connection_tunnel(size_t id, b_asio::io_service& ioservice,
                      unique_ptr<b_ip::tcp::socket> &&clientSocket, b_ip::tcp::endpoint server);

    void connectionClosedCallback(callback_t callback);
    std::string idString();
    void closeAllConnections();

public:
    inline size_t id() const  { return _id; }
};

//----------------------------------------------------------------------------------------------------------------------

#endif    //SFERA_PROXY_CONNECTION_TUNNEL_H
