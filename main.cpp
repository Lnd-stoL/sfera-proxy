
#include <iostream>
#include "load_balancing_proxy.h"

//----------------------------------------------------------------------------------------------------------------------

bool readConfig(const std::string& fileName, unsigned &port, std::vector<b_ip::tcp::endpoint> &servers)
{
    std::ifstream configFile(fileName);
    if (!configFile.good()) return false;

    std::string strPort;
    std::getline(configFile, strPort, ',');
    boost::trim(strPort);

    port = 0; port = (unsigned) std::atoi(strPort.c_str());
    if (port == 0) {
        configFile.close();
        return false;
    }

    servers.clear();
    while (!configFile.eof()) {

        std::string strNextServer;
        std::getline(configFile, strNextServer, ',');
        boost::trim_if(strNextServer, boost::is_any_of(" \n\r"));

        auto separatorIt = std::find(strNextServer.begin(), strNextServer.end(), ':');
        if (separatorIt == strNextServer.end()) {
            configFile.close();
            return false;
        }

        std::string serverIp(strNextServer.begin(), separatorIt);
        std::string nextSrvPortStr(separatorIt+1, strNextServer.end());

        if (serverIp.empty() || nextSrvPortStr.empty()) {
            configFile.close();
            return false;
        }

        unsigned nextSrvPort = 0;
        nextSrvPort = (unsigned) std::atoi(nextSrvPortStr.c_str());
        if (nextSrvPort == 0)  {
            configFile.close();
            return false;
        }

        servers.push_back(b_ip::tcp::endpoint(b_ip::address::from_string(serverIp), (unsigned short)nextSrvPort));
    }

    configFile.close();
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "error: configuration file not provided (no arguments)" << std::endl;
        std::cout << "<exiting>" << std::endl;
        return 0;
    }

    unsigned port = 0;
    std::vector<b_ip::tcp::endpoint> serversList;
    std::string configFileName(argv[1]);
    if (!readConfig(configFileName, port, serversList)) {
        std::cout << "error: configuration file doesn't exist or invalid, parsing failed" << std::endl;
        std::cout << "<exiting>" << std::endl;
        return 0;
    }

    load_balancing_proxy proxy("0.0.0.0", port);
    for (auto server : serversList) {
        std::cout << "proxy: adding worker server listening at " <<
                server.address().to_string() << ":" << server.port() << std::endl;
        proxy.addServer(server);
    }
    proxy.run();

    std::cout << "<exiting>" << std::endl;
    return 0;
}
