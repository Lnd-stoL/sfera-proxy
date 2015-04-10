
#ifndef SFERA_PROXY_LOAD_BALANCER_H
#define SFERA_PROXY_LOAD_BALANCER_H

//----------------------------------------------------------------------------------------------------------------------

#include "connection_tunnel.h"

#include <vector>
#include <unordered_map>

//----------------------------------------------------------------------------------------------------------------------

class load_balancing_proxy
{
private:
    b_asio::io_service _ioservice;
    unique_ptr<b_ip::tcp::socket> _acceptedSocket;
    b_ip::tcp::acceptor _acceptor;

    std::vector<b_ip::tcp::endpoint> _serversList;
    std::unordered_map<size_t, shared_ptr<connection_tunnel>> _tunnels;
    size_t _tunnelId = 0;


private:
    void _startAsyncAccept();
    void _handleAccept(const b_sys::error_code &ec);
    void _handleTunnelClose(connection_tunnel* tunnel);

public:
    load_balancing_proxy(const std::string &ip, int port);
    ~load_balancing_proxy();

    void addServer(const b_ip::tcp::endpoint& srv);
    void run();

public:
    inline const b_asio::io_service&  ioService() const  { return _ioservice; }
};

//----------------------------------------------------------------------------------------------------------------------

#endif    //SFERA_PROXY_LOAD_BALANCER_H
