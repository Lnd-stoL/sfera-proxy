
#include "load_balancing_proxy.h"

//----------------------------------------------------------------------------------------------------------------------

load_balancing_proxy::load_balancing_proxy(const std::string &ip, int port) :
    _acceptor(_ioservice, b_ip::tcp::endpoint(b_ip::address::from_string(ip), (unsigned short)port), true)
{
    std::cout << "starting load-balancing proxy at " << ip << ":" << port;

    _acceptor.listen();
    _startAsyncAccept();

    std::cout << " \t[OK]" << std::endl;
}


load_balancing_proxy::~load_balancing_proxy()
{
}


void load_balancing_proxy::run()
{
    _ioservice.run();
}


void load_balancing_proxy::_startAsyncAccept()
{
    _acceptedSocket.reset(new b_ip::tcp::socket(_ioservice));
    _acceptor.async_accept(*_acceptedSocket,
                           boost::bind(&load_balancing_proxy::_handleAccept, this, b_asio::placeholders::error));
}


void load_balancing_proxy::_handleAccept(const b_sys::error_code &ec)
{
    if (ec) {
        std::cout << "accepting connection failed: " << ec.message() << std::endl;
        _startAsyncAccept();
        return;
    }

    auto remoteEndpoint =_acceptedSocket->remote_endpoint();
    std::cout << "accepted connection from " << remoteEndpoint.address().to_string() << ":"
    << remoteEndpoint.port() << std::endl;

    auto randServer = _serversList[rand() % _serversList.size()];
    auto tunnel = make_shared<connection_tunnel>(++_tunnelId, _ioservice, std::move(_acceptedSocket), randServer);

    tunnel->connectionClosedCallback([this] (connection_tunnel* ct) { this->_handleTunnelClose(ct); });
    _tunnels[_tunnelId] = tunnel;

    _startAsyncAccept();
}


void load_balancing_proxy::addServer(const b_ip::tcp::endpoint &srv)
{
    _serversList.push_back(srv);
}


void load_balancing_proxy::_handleTunnelClose(connection_tunnel *tunnel)
{

    _tunnels.erase(tunnel->id());
}
