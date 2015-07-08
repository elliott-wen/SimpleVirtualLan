cmake_minimum_required(VERSION 3.2)
project(Catcher)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


add_executable(Client CovertTap.h CovertTap.cpp CovertNetwork.cpp CovertNetwork.h CovertSecret.cpp CovertSecret.h CovertDevice.cpp CovertDevice.h CovertUtils.cpp CovertUtils.h)
target_link_libraries(Client wsock32 ws2_32 libz)

add_executable(Switch CovertSwitch.cpp CovertSwitch.h CovertSecret.cpp CovertSecret.h CovertUtils.cpp CovertUtils.h)
target_link_libraries(Switch wsock32 ws2_32 libz)