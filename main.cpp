
#include <iostream>
#include "load_balancing_proxy.h"

//----------------------------------------------------------------------------------------------------------------------

int main()
{
    load_balancing_proxy proxy("127.0.0.1", 8888);
    proxy.addServer(b_ip::tcp::endpoint(b_ip::address::from_string("127.0.0.1"), 8080));
    proxy.run();

    std::cout << "proxy terminated" << std::endl;
    return 0;
}
