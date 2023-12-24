cmake_minimum_required(VERSION 2.8.12.2)

add_compile_options(-std=c++11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

__cc_find(zlib_static)
__cc_find(curl_static)
set(ILIBS zlib_static curl_static)
__add_real_target(sensors_analytics_sdk lib SOURCE src/sensors_analytics_sdk.cpp src/sensors_network.cpp src/sensors_utils.cpp
											ILIB ${ILIBS}
											INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/
											)

if(CC_INSTALL_SENSORS_ANALYTICS_SDK)
	__install_directory_specif(${CMAKE_CURRENT_SOURCE_DIR}/include/ sensors_analytics_sdk)
endif()
