cmake_minimum_required(VERSION 3.24)
project(weather_service)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(zeromq CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)


add_executable(weather_service weatherService.cpp)

target_link_libraries(weather_service PRIVATE
        nlohmann_json::nlohmann_json
        cppzmq
        libzmq
        cpr::cpr
)