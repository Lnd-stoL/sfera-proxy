
#ifndef SFERA_WEBSERVER_ASIO_HEADERS_H
#define SFERA_WEBSERVER_ASIO_HEADERS_H

//----------------------------------------------------------------------------------------------------------------------

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

#include <string>
#include <memory>

namespace b_time = boost::posix_time;
namespace b_fs   = boost::filesystem;
namespace b_sys  = boost::system;
namespace b_asio = boost::asio;
namespace b_ip   = boost::asio::ip;

using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;

//----------------------------------------------------------------------------------------------------------------------

#endif   //SFERA_WEBSERVER_ASIO_HEADERS_H
