
#include "connection_tunnel.h"

//----------------------------------------------------------------------------------------------------------------------

#define _ERROR_LOCATION  (__PRETTY_FUNCTION__)

//----------------------------------------------------------------------------------------------------------------------

connection_tunnel::connection_tunnel(size_t id, b_asio::io_service &ioservice,
                                     unique_ptr<b_ip::tcp::socket> &&clientSocket,
                                     b_ip::tcp::endpoint server) :
    _id(id),
    _clientSocket((unique_ptr<b_ip::tcp::socket> &&)clientSocket)  //todo: WTF????? why type cast here is necessary?
{
    _serverSocket.reset(new b_ip::tcp::socket(ioservice));
    ++_pendingAsyncOperations;
    _serverSocket->async_connect(server,
                                 [this] (const b_sys::error_code &ec) { _handleConnectedToServer(ec); });
}


void connection_tunnel::connectionClosedCallback(connection_tunnel::callback_t callback)
{
    _connectionClosedCallback = callback;
}


void connection_tunnel::_handleConnectedToServer(const b_sys::error_code &ec)
{
    --_pendingAsyncOperations;

    if (ec) {
        _onError(ec, _ERROR_LOCATION);
        return;
    }

    std::cout << idString() << "successfully connected to server at " << _serverSocket->remote_endpoint().address()
            << ":" << _serverSocket->remote_endpoint().port() << std::endl;

    _startAsyncClientRead();
    _startAsyncServerRead();
}


std::string connection_tunnel::idString()
{
    return std::string("tunnel#") +  std::to_string(_id) + ": ";
}


void connection_tunnel::closeAllConnections()
{
    std::cout << idString() << "closing tcp connection ..." << std::endl;

    try {
        _serverSocket->shutdown(b_ip::tcp::socket::shutdown_type::shutdown_both);
        _clientSocket->shutdown(b_ip::tcp::socket::shutdown_type::shutdown_both);

        _clientSocket->close();
        _serverSocket->close();

    } catch (const b_sys::system_error &err) {
        std::cout << idString() << "failed to close connection properly: " << err.what() << std::endl;
    }

    std::cout << idString() << "connection terminated; sockets are closed" << std::endl;
}


void connection_tunnel::_handleClientRead(const b_sys::error_code &ec, size_t bytesTransferred)
{
    --_pendingAsyncOperations;

    if (ec) {
        _onError(ec, _ERROR_LOCATION);
        return;
    }

    _clientBuff.commit(bytesTransferred);
    _startAsyncServerWrite();
    _startAsyncClientRead();
}


void connection_tunnel::_onError(const b_sys::error_code &ec, const std::string &location)
{
    if (!_closing) {
        std::cout << idString() << "error: " << ec.message()  << "  at [ " << location << " ]" << std::endl;

        _closing = true;
        _serverSocket->cancel();
        _clientSocket->cancel();
    }

    if (!_pendingAsyncOperations) {
        closeAllConnections();
        _connectionClosedCallback(this);
    }
}


void connection_tunnel::_handleServerWrite(const b_sys::error_code &ec, size_t bytesTransferred)
{
    --_pendingAsyncOperations;

    if (ec) {
        _onError(ec, _ERROR_LOCATION);
        return;
    }

    _clientBuff.consume(bytesTransferred);
    if (_clientBuff.size() > 0) {
        _startAsyncServerWrite();
    }
}


void connection_tunnel::_startAsyncClientRead()
{
    _clientSocket->async_read_some(_clientBuff.prepare(1024),
                                   boost::bind(&connection_tunnel::_handleClientRead,
                                               this, b_asio::placeholders::error,
                                               b_asio::placeholders::bytes_transferred));
    ++_pendingAsyncOperations;
}


void connection_tunnel::_startAsyncServerWrite()
{
    _serverSocket->async_write_some(_clientBuff.data(),
                                    boost::bind(&connection_tunnel::_handleServerWrite,
                                                this, b_asio::placeholders::error,
                                                b_asio::placeholders::bytes_transferred));
    ++_pendingAsyncOperations;
}


void connection_tunnel::_startAsyncServerRead()
{
    _serverSocket->async_read_some(_serverBuff.prepare(1024),
                                   boost::bind(&connection_tunnel::_handleServerRead,
                                               this, b_asio::placeholders::error,
                                               b_asio::placeholders::bytes_transferred));
    ++_pendingAsyncOperations;
}


void connection_tunnel::_startAsyncClientWrite()
{
    _clientSocket->async_write_some(_serverBuff.data(),
                                    boost::bind(&connection_tunnel::_handleClientWrite,
                                                this, b_asio::placeholders::error,
                                                b_asio::placeholders::bytes_transferred));
    ++_pendingAsyncOperations;
}


void connection_tunnel::_handleClientWrite(const b_sys::error_code &ec, size_t bytesTransferred)
{
    --_pendingAsyncOperations;

    if (ec) {
        _onError(ec, _ERROR_LOCATION);
        return;
    }

    _serverBuff.consume(bytesTransferred);
    if (_serverBuff.size() > 0) {
        _startAsyncClientWrite();
    }
}


void connection_tunnel::_handleServerRead(const b_sys::error_code &ec, size_t bytesTransferred)
{
    --_pendingAsyncOperations;

    if (ec) {
        _onError(ec, _ERROR_LOCATION);
        return;
    }

    _serverBuff.commit(bytesTransferred);
    _startAsyncClientWrite();
    _startAsyncServerRead();
}
