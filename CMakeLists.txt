cmake_minimum_required(VERSION 3.15)

project(curl-learning)

SET(CONAN_DISABLE_CHECK_COMPILER TRUE)

find_package(CURL REQUIRED)

add_executable(curl_downloader curl.cpp)
target_link_libraries(curl_downloader CURL::libcurl)