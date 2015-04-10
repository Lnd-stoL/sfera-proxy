cmake_minimum_required(VERSION 3.1)
project(sfera_proxy)

find_package(Boost 1.5.0 COMPONENTS system filesystem program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(sfera_proxy ${SOURCE_FILES} load_balancing_proxy.cpp load_balancing_proxy.h boost_asio_headers.h connection_tunnel.cpp connection_tunnel.h)
target_link_libraries(sfera_proxy ${Boost_LIBRARIES})
