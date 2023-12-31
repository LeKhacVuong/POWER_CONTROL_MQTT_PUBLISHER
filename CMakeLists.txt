cmake_minimum_required(VERSION 3.26)
project(PROJECT2_MQTT)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

#thêm thư mục chứa thư viện
include_directories(host utils)

# Tạo thư viện tĩnh từ các tệp trong thư mục lib

set( MY_LIB
        host/sm_cmd.h
        host/sm_host.c
        host/sm_host.h
        utils/CRC.c
        utils/CRC.h
)

#set(MY_LIB ${MY_LIB}
#        utils/linux/linux_serial.h
#        utils/linux/linux_serial.c)


list(APPEND MY_VAR ON)


if(MY_VAR)
    list(APPEND MY_LIB
            utils/linux/linux_serial.h
            utils/linux/linux_serial.c)
endif ()



add_library(MyLibrary STATIC ${MY_LIB})

add_executable(PROJECT2_MQTT main.cpp)
target_link_libraries(PROJECT2_MQTT PRIVATE MyLibrary pthread libmosquitto)

add_subdirectory(lib)
add_subdirectory(src)
